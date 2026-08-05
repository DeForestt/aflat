#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <linux/io_uring.h>
#include <poll.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/eventfd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

typedef struct af_worker_thread {
  pthread_mutex_t mutex;
  pthread_t thread;
  void *(*function)(void *, void *, void *, void *);
  void *arguments[4];
  void *result;
  int finished;
  int joined;
  int references;
  int released;
} af_worker_thread;

typedef struct af_worker_timeout {
  af_worker_thread *worker;
  struct timespec duration;
} af_worker_timeout;

static void af_worker_thread_finished(void *opaque) {
  af_worker_thread *worker = opaque;
  pthread_mutex_lock(&worker->mutex);
  worker->finished = 1;
  pthread_mutex_unlock(&worker->mutex);
}

/* AFlat functions currently use the System V callee-saved registers as
 * scratch registers. Preserve them around a callback into generated code so
 * the surrounding C pthread trampoline remains ABI-correct. */
__attribute__((naked, noinline)) static void *
af_worker_thread_invoke(void *(*function)(void *, void *, void *, void *),
                        void *arg1, void *arg2, void *arg3, void *arg4) {
  __asm__("pushq %rbp\n\t"
          "movq %rsp, %rbp\n\t"
          "pushq %rbx\n\t"
          "pushq %r12\n\t"
          "pushq %r13\n\t"
          "pushq %r14\n\t"
          "pushq %r15\n\t"
          "subq $8, %rsp\n\t"
          "movq %rdi, %rax\n\t"
          "movq %rsi, %rdi\n\t"
          "movq %rdx, %rsi\n\t"
          "movq %rcx, %rdx\n\t"
          "movq %r8, %rcx\n\t"
          "call *%rax\n\t"
          "addq $8, %rsp\n\t"
          "popq %r15\n\t"
          "popq %r14\n\t"
          "popq %r13\n\t"
          "popq %r12\n\t"
          "popq %rbx\n\t"
          "leave\n\t"
          "ret");
}

static void *af_worker_thread_run(void *opaque) {
  af_worker_thread *worker = opaque;
  void *result = NULL;
  pthread_cleanup_push(af_worker_thread_finished, worker);
  result = af_worker_thread_invoke(worker->function, worker->arguments[0],
                                   worker->arguments[1], worker->arguments[2],
                                   worker->arguments[3]);
  pthread_cleanup_pop(0);

  pthread_mutex_lock(&worker->mutex);
  worker->result = result;
  worker->finished = 1;
  pthread_mutex_unlock(&worker->mutex);
  return result;
}

static void af_worker_thread_free(af_worker_thread *worker) {
  pthread_mutex_destroy(&worker->mutex);
  munmap(worker, sizeof(*worker));
}

void *af_worker_thread_create(void *function, void *arg1, void *arg2,
                              void *arg3, void *arg4) {
  af_worker_thread *worker = mmap(NULL, sizeof(*worker), PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (worker == MAP_FAILED)
    return NULL;

  pthread_mutex_init(&worker->mutex, NULL);
  worker->function = (void *(*)(void *, void *, void *, void *))function;
  worker->arguments[0] = arg1;
  worker->arguments[1] = arg2;
  worker->arguments[2] = arg3;
  worker->arguments[3] = arg4;
  worker->references = 1;
  if (pthread_create(&worker->thread, NULL, af_worker_thread_run, worker) !=
      0) {
    af_worker_thread_free(worker);
    return NULL;
  }
  return worker;
}

void *af_worker_thread_await(void *opaque) {
  af_worker_thread *worker = opaque;
  if (worker == NULL)
    return NULL;

  pthread_mutex_lock(&worker->mutex);
  const int should_join = !worker->joined;
  worker->joined = 1;
  pthread_mutex_unlock(&worker->mutex);
  if (should_join)
    pthread_join(worker->thread, NULL);

  pthread_mutex_lock(&worker->mutex);
  void *result = worker->result;
  pthread_mutex_unlock(&worker->mutex);
  return result;
}

int af_worker_thread_is_running(void *opaque) {
  af_worker_thread *worker = opaque;
  if (worker == NULL)
    return 0;
  pthread_mutex_lock(&worker->mutex);
  const int running = !worker->finished;
  pthread_mutex_unlock(&worker->mutex);
  return running;
}

int af_worker_thread_cancel(void *opaque) {
  af_worker_thread *worker = opaque;
  if (worker == NULL)
    return -1;
  pthread_mutex_lock(&worker->mutex);
  const int finished = worker->finished;
  pthread_mutex_unlock(&worker->mutex);
  return finished ? 0 : pthread_cancel(worker->thread);
}

static void *af_worker_thread_timeout_run(void *opaque) {
  af_worker_timeout *timeout = opaque;
  nanosleep(&timeout->duration, NULL);
  af_worker_thread_cancel(timeout->worker);

  pthread_mutex_lock(&timeout->worker->mutex);
  timeout->worker->references--;
  const int should_free =
      timeout->worker->released && timeout->worker->references == 0;
  pthread_mutex_unlock(&timeout->worker->mutex);
  if (should_free)
    af_worker_thread_free(timeout->worker);
  munmap(timeout, sizeof(*timeout));
  return NULL;
}

int af_worker_thread_timeout(void *opaque, int seconds, int nanoseconds) {
  af_worker_thread *worker = opaque;
  if (worker == NULL)
    return -1;

  af_worker_timeout *timeout =
      mmap(NULL, sizeof(*timeout), PROT_READ | PROT_WRITE,
           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (timeout == MAP_FAILED)
    return -1;
  timeout->worker = worker;
  timeout->duration.tv_sec = seconds;
  timeout->duration.tv_nsec = nanoseconds;

  pthread_mutex_lock(&worker->mutex);
  worker->references++;
  pthread_mutex_unlock(&worker->mutex);

  pthread_t timer;
  const int status =
      pthread_create(&timer, NULL, af_worker_thread_timeout_run, timeout);
  if (status != 0) {
    pthread_mutex_lock(&worker->mutex);
    worker->references--;
    pthread_mutex_unlock(&worker->mutex);
    munmap(timeout, sizeof(*timeout));
    return status;
  }
  pthread_detach(timer);
  return 0;
}

void af_worker_thread_release(void *opaque) {
  af_worker_thread *worker = opaque;
  if (worker == NULL)
    return;
  af_worker_thread_cancel(worker);
  af_worker_thread_await(worker);

  pthread_mutex_lock(&worker->mutex);
  worker->released = 1;
  worker->references--;
  const int should_free = worker->references == 0;
  pthread_mutex_unlock(&worker->mutex);
  if (should_free)
    af_worker_thread_free(worker);
}

#if defined(__has_include)
#if __has_include(<valgrind/memcheck.h>)
#include <valgrind/memcheck.h>
#define AF_MARK_MEMORY_DEFINED(address, length)                                \
  VALGRIND_MAKE_MEM_DEFINED((address), (length))
#endif
#endif
#ifndef AF_MARK_MEMORY_DEFINED
#define AF_MARK_MEMORY_DEFINED(address, length) ((void)0)
#endif

/*
 * The default AFlat allocator is implemented in std.af as a process-wide
 * linked-list arena.  Async tasks run on a worker thread, so allocator list
 * traversal and mutation must be serialized with allocations on the caller
 * thread.  The mutex is recursive because af_realloc delegates to af_malloc
 * and af_free while holding the allocator lock.
 */
static pthread_once_t af_allocator_once = PTHREAD_ONCE_INIT;
static pthread_mutex_t af_allocator_mutex;

static void af_allocator_init(void) {
  pthread_mutexattr_t attributes;
  pthread_mutexattr_init(&attributes);
  pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&af_allocator_mutex, &attributes);
  pthread_mutexattr_destroy(&attributes);
}

enum af_task_status {
  AF_TASK_CREATED,
  AF_TASK_SCHEDULED,
  AF_TASK_RUNNING,
  AF_TASK_WAITING,
  AF_TASK_SUSPENDED,
  AF_TASK_COMPLETED,
  AF_TASK_FAILED,
  AF_TASK_CANCELLED
};

typedef struct af_task af_task;
typedef struct af_waiter af_waiter;
typedef struct af_timer af_timer;
typedef struct af_io_operation af_io_operation;

typedef struct af_io_ring {
  int fd;
  void *sq_mapping;
  size_t sq_mapping_size;
  void *cq_mapping;
  size_t cq_mapping_size;
  struct io_uring_sqe *sqes;
  size_t sqes_size;
  unsigned *sq_head;
  unsigned *sq_tail;
  unsigned *sq_mask;
  unsigned *sq_entries;
  unsigned *sq_array;
  unsigned *cq_head;
  unsigned *cq_tail;
  unsigned *cq_mask;
  struct io_uring_cqe *cqes;
} af_io_ring;

struct af_waiter {
  af_task *task;
  af_waiter *next;
};

struct af_task {
  pthread_mutex_t mutex;
  pthread_cond_t changed;
  void *resume;
  uint64_t args[6];
  uint64_t result;
  unsigned char *frame;
  size_t frame_size;
  af_task *awaited;
  af_waiter *waiters;
  af_task *queue_next;
  enum af_task_status status;
  int state;
  unsigned char argc;
  unsigned char cancel_requested;
  unsigned char detached;
  unsigned char observed;
  unsigned char wake_pending;
  af_io_operation *pending_io;
  int64_t io_begin_error;
};

struct af_timer {
  struct timespec deadline;
  af_task *task;
  af_timer *next;
};

struct af_io_operation {
  af_task *task;
  void *buffer;
  unsigned char opcode;
  _Atomic int completed;
  _Atomic int abandoned;
  _Atomic int64_t result;
};

typedef struct af_runtime {
  pthread_mutex_t mutex;
  pthread_cond_t ready;
  pthread_t worker;
  af_task *queue_head;
  af_task *queue_tail;
  af_timer *timers;
  int wake_fd;
  af_io_ring io;
} af_runtime;

static af_runtime af_global_runtime;
static pthread_once_t af_runtime_once = PTHREAD_ONCE_INIT;
static _Thread_local af_runtime *af_current_runtime;
static _Thread_local af_task *af_current_task;

static void af_task_wake_internal(af_task *task);

#if defined(__x86_64__)
#define AF_ENTRY __attribute__((force_align_arg_pointer))
#else
#define AF_ENTRY
#endif

AF_ENTRY void af_allocator_lock(void) {
  pthread_once(&af_allocator_once, af_allocator_init);
  pthread_mutex_lock(&af_allocator_mutex);
}

AF_ENTRY void af_allocator_unlock(void) {
  pthread_mutex_unlock(&af_allocator_mutex);
}

static int af_terminal(enum af_task_status status) {
  return status == AF_TASK_COMPLETED || status == AF_TASK_FAILED ||
         status == AF_TASK_CANCELLED;
}

static int af_timespec_compare(const struct timespec *a,
                               const struct timespec *b) {
  if (a->tv_sec != b->tv_sec)
    return a->tv_sec < b->tv_sec ? -1 : 1;
  if (a->tv_nsec == b->tv_nsec)
    return 0;
  return a->tv_nsec < b->tv_nsec ? -1 : 1;
}

static int af_timeout_milliseconds(const struct timespec *deadline) {
  if (deadline == NULL)
    return -1;
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  int64_t nanoseconds =
      (int64_t)(deadline->tv_sec - now.tv_sec) * 1000000000LL +
      (int64_t)deadline->tv_nsec - now.tv_nsec;
  if (nanoseconds <= 0)
    return 0;
  int64_t milliseconds = (nanoseconds + 999999LL) / 1000000LL;
  return milliseconds > INT32_MAX ? INT32_MAX : (int)milliseconds;
}

static void af_notify_scheduler(af_runtime *runtime) {
  if (runtime->wake_fd < 0)
    return;
  uint64_t signal = 1;
  ssize_t written = write(runtime->wake_fd, &signal, sizeof(signal));
  (void)written;
}

static void af_drain_scheduler_notifications(af_runtime *runtime) {
  if (runtime->wake_fd < 0)
    return;
  uint64_t value;
  while (read(runtime->wake_fd, &value, sizeof(value)) == sizeof(value)) {
  }
}

static void af_io_ring_reset(af_io_ring *ring) {
  memset(ring, 0, sizeof(*ring));
  ring->fd = -1;
}

static int af_io_ring_init(af_io_ring *ring, unsigned entries) {
  af_io_ring_reset(ring);
  struct io_uring_params parameters;
  memset(&parameters, 0, sizeof(parameters));
  int fd = (int)syscall(__NR_io_uring_setup, entries, &parameters);
  if (fd < 0)
    return -errno;

  size_t sq_size =
      parameters.sq_off.array + parameters.sq_entries * sizeof(unsigned);
  size_t cq_size = parameters.cq_off.cqes +
                   parameters.cq_entries * sizeof(struct io_uring_cqe);
  if (parameters.features & IORING_FEAT_SINGLE_MMAP) {
    size_t mapping_size = sq_size > cq_size ? sq_size : cq_size;
    void *mapping = mmap(NULL, mapping_size, PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_POPULATE, fd, IORING_OFF_SQ_RING);
    if (mapping == MAP_FAILED) {
      int error = -errno;
      close(fd);
      return error;
    }
    ring->sq_mapping = mapping;
    ring->cq_mapping = mapping;
    ring->sq_mapping_size = mapping_size;
    ring->cq_mapping_size = mapping_size;
  } else {
    ring->sq_mapping = mmap(NULL, sq_size, PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_POPULATE, fd, IORING_OFF_SQ_RING);
    if (ring->sq_mapping == MAP_FAILED) {
      int error = -errno;
      close(fd);
      af_io_ring_reset(ring);
      return error;
    }
    ring->sq_mapping_size = sq_size;
    ring->cq_mapping = mmap(NULL, cq_size, PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_POPULATE, fd, IORING_OFF_CQ_RING);
    if (ring->cq_mapping == MAP_FAILED) {
      int error = -errno;
      munmap(ring->sq_mapping, ring->sq_mapping_size);
      close(fd);
      af_io_ring_reset(ring);
      return error;
    }
    ring->cq_mapping_size = cq_size;
  }

  ring->sqes_size = parameters.sq_entries * sizeof(struct io_uring_sqe);
  ring->sqes = mmap(NULL, ring->sqes_size, PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_POPULATE, fd, IORING_OFF_SQES);
  if (ring->sqes == MAP_FAILED) {
    int error = -errno;
    if (ring->cq_mapping != ring->sq_mapping)
      munmap(ring->cq_mapping, ring->cq_mapping_size);
    munmap(ring->sq_mapping, ring->sq_mapping_size);
    close(fd);
    af_io_ring_reset(ring);
    return error;
  }

  unsigned char *sq = ring->sq_mapping;
  unsigned char *cq = ring->cq_mapping;
  ring->sq_head = (unsigned *)(sq + parameters.sq_off.head);
  ring->sq_tail = (unsigned *)(sq + parameters.sq_off.tail);
  ring->sq_mask = (unsigned *)(sq + parameters.sq_off.ring_mask);
  ring->sq_entries = (unsigned *)(sq + parameters.sq_off.ring_entries);
  ring->sq_array = (unsigned *)(sq + parameters.sq_off.array);
  ring->cq_head = (unsigned *)(cq + parameters.cq_off.head);
  ring->cq_tail = (unsigned *)(cq + parameters.cq_off.tail);
  ring->cq_mask = (unsigned *)(cq + parameters.cq_off.ring_mask);
  ring->cqes = (struct io_uring_cqe *)(cq + parameters.cq_off.cqes);
  ring->fd = fd;
  return 0;
}

static int af_io_submit(af_runtime *runtime, af_io_operation *operation,
                        unsigned char opcode, int descriptor, void *buffer,
                        size_t length, int64_t offset) {
  af_io_ring *ring = &runtime->io;
  if (ring->fd < 0)
    return -ENOSYS;
  unsigned head = __atomic_load_n(ring->sq_head, __ATOMIC_ACQUIRE);
  unsigned tail = __atomic_load_n(ring->sq_tail, __ATOMIC_RELAXED);
  if (tail - head >= *ring->sq_entries)
    return -EAGAIN;
  unsigned index = tail & *ring->sq_mask;
  struct io_uring_sqe *entry = &ring->sqes[index];
  memset(entry, 0, sizeof(*entry));
  entry->opcode = opcode;
  entry->fd = descriptor;
  entry->off = (uint64_t)offset;
  entry->addr = (uint64_t)(uintptr_t)buffer;
  entry->len = length > UINT32_MAX ? UINT32_MAX : (uint32_t)length;
  entry->user_data = (uint64_t)(uintptr_t)operation;
  ring->sq_array[index] = index;
  __atomic_store_n(ring->sq_tail, tail + 1, __ATOMIC_RELEASE);

  int submitted;
  do {
    submitted =
        (int)syscall(__NR_io_uring_enter, ring->fd, 1u, 0u, 0u, NULL, 0u);
  } while (submitted < 0 && errno == EINTR);
  if (submitted < 0) {
    __atomic_store_n(ring->sq_tail, tail, __ATOMIC_RELEASE);
    return -errno;
  }
  return 0;
}

static void af_io_complete(af_io_operation *operation, int result) {
  af_task *task = operation->task;
  pthread_mutex_lock(&task->mutex);
  if (operation->opcode == IORING_OP_READ && result > 0)
    AF_MARK_MEMORY_DEFINED(operation->buffer, (size_t)result);
  atomic_store_explicit(&operation->result, result, memory_order_relaxed);
  atomic_store_explicit(&operation->completed, 1, memory_order_release);
  int abandoned =
      atomic_load_explicit(&operation->abandoned, memory_order_acquire);
  if (abandoned && task->pending_io == operation)
    task->pending_io = NULL;
  pthread_mutex_unlock(&task->mutex);
  if (abandoned) {
    free(operation);
    return;
  }
  af_task_wake_internal(task);
}

static void af_io_drain_completions(af_runtime *runtime) {
  af_io_ring *ring = &runtime->io;
  if (ring->fd < 0)
    return;
  unsigned head = __atomic_load_n(ring->cq_head, __ATOMIC_RELAXED);
  unsigned tail = __atomic_load_n(ring->cq_tail, __ATOMIC_ACQUIRE);
  while (head != tail) {
    struct io_uring_cqe *entry = &ring->cqes[head & *ring->cq_mask];
    af_io_operation *operation = (af_io_operation *)(uintptr_t)entry->user_data;
    if (operation != NULL)
      af_io_complete(operation, entry->res);
    ++head;
  }
  __atomic_store_n(ring->cq_head, head, __ATOMIC_RELEASE);
}

static void af_enqueue_locked(af_runtime *runtime, af_task *task) {
  task->queue_next = NULL;
  if (runtime->queue_tail == NULL)
    runtime->queue_head = runtime->queue_tail = task;
  else {
    runtime->queue_tail->queue_next = task;
    runtime->queue_tail = task;
  }
  pthread_cond_signal(&runtime->ready);
  af_notify_scheduler(runtime);
}

static void af_schedule(af_task *task) {
  if (task == NULL)
    return;
  pthread_mutex_lock(&task->mutex);
  if (task->status != AF_TASK_CREATED && task->status != AF_TASK_WAITING) {
    pthread_mutex_unlock(&task->mutex);
    return;
  }
  task->status = AF_TASK_SCHEDULED;
  pthread_mutex_unlock(&task->mutex);
  pthread_mutex_lock(&af_global_runtime.mutex);
  af_enqueue_locked(&af_global_runtime, task);
  pthread_mutex_unlock(&af_global_runtime.mutex);
}

static void af_wake_waiters(af_task *task) {
  pthread_mutex_lock(&task->mutex);
  af_waiter *waiters = task->waiters;
  task->waiters = NULL;
  pthread_cond_broadcast(&task->changed);
  pthread_mutex_unlock(&task->mutex);
  while (waiters != NULL) {
    af_waiter *next = waiters->next;
    af_schedule(waiters->task);
    free(waiters);
    waiters = next;
  }
}

static void af_finish(af_task *task, enum af_task_status status,
                      uint64_t result) {
  pthread_mutex_lock(&task->mutex);
  if (!af_terminal(task->status)) {
    task->result = result;
    task->status = status;
  }
  pthread_mutex_unlock(&task->mutex);
  af_wake_waiters(task);
}

static uint64_t af_invoke(af_task *task) {
#if defined(__x86_64__)
  /* AFlat-generated functions currently use these registers internally. */
  __asm__ volatile("" ::: "rbx", "r12", "r13", "r14", "r15");
#endif
  switch (task->argc) {
  case 0:
    return ((uint64_t(*)(void))task->resume)();
  case 1:
    return ((uint64_t(*)(uint64_t))task->resume)(task->args[0]);
  case 2:
    return ((uint64_t(*)(uint64_t, uint64_t))task->resume)(task->args[0],
                                                           task->args[1]);
  case 3:
    return ((uint64_t(*)(uint64_t, uint64_t, uint64_t))task->resume)(
        task->args[0], task->args[1], task->args[2]);
  case 4:
    return ((uint64_t(*)(uint64_t, uint64_t, uint64_t, uint64_t))task->resume)(
        task->args[0], task->args[1], task->args[2], task->args[3]);
  case 5:
    return ((uint64_t(*)(uint64_t, uint64_t, uint64_t, uint64_t,
                         uint64_t))task->resume)(task->args[0], task->args[1],
                                                 task->args[2], task->args[3],
                                                 task->args[4]);
  default:
    return ((uint64_t(*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t,
                         uint64_t))task->resume)(task->args[0], task->args[1],
                                                 task->args[2], task->args[3],
                                                 task->args[4], task->args[5]);
  }
}

static void af_task_wake_internal(af_task *task) {
  if (task == NULL)
    return;
  pthread_mutex_lock(&task->mutex);
  if (af_terminal(task->status)) {
    pthread_mutex_unlock(&task->mutex);
    return;
  }
  if (task->status == AF_TASK_SUSPENDED) {
    task->status = AF_TASK_SCHEDULED;
    pthread_mutex_unlock(&task->mutex);
    pthread_mutex_lock(&af_global_runtime.mutex);
    af_enqueue_locked(&af_global_runtime, task);
    pthread_mutex_unlock(&af_global_runtime.mutex);
    return;
  }
  task->wake_pending = 1;
  pthread_mutex_unlock(&task->mutex);
}

static void *af_runtime_worker(void *unused) {
  (void)unused;
  af_current_runtime = &af_global_runtime;
  for (;;) {
    pthread_mutex_lock(&af_global_runtime.mutex);
    af_task *task = NULL;
    while (task == NULL) {
      pthread_mutex_unlock(&af_global_runtime.mutex);
      af_io_drain_completions(&af_global_runtime);
      pthread_mutex_lock(&af_global_runtime.mutex);
      struct timespec now;
      clock_gettime(CLOCK_MONOTONIC, &now);
      if (af_global_runtime.timers != NULL &&
          af_timespec_compare(&af_global_runtime.timers->deadline, &now) <= 0) {
        af_timer *timer = af_global_runtime.timers;
        af_global_runtime.timers = timer->next;
        pthread_mutex_unlock(&af_global_runtime.mutex);
        af_task_wake_internal(timer->task);
        free(timer);
        pthread_mutex_lock(&af_global_runtime.mutex);
        continue;
      }
      if (af_global_runtime.queue_head != NULL) {
        task = af_global_runtime.queue_head;
        af_global_runtime.queue_head = task->queue_next;
        if (af_global_runtime.queue_head == NULL)
          af_global_runtime.queue_tail = NULL;
        task->queue_next = NULL;
        break;
      }
      if (af_global_runtime.io.fd >= 0) {
        int timeout =
            af_timeout_milliseconds(af_global_runtime.timers == NULL
                                        ? NULL
                                        : &af_global_runtime.timers->deadline);
        struct pollfd descriptors[2] = {
            {.fd = af_global_runtime.wake_fd, .events = POLLIN},
            {.fd = af_global_runtime.io.fd, .events = POLLIN},
        };
        pthread_mutex_unlock(&af_global_runtime.mutex);
        int ready;
        do {
          ready = poll(descriptors, 2, timeout);
        } while (ready < 0 && errno == EINTR);
        if (descriptors[0].revents & POLLIN)
          af_drain_scheduler_notifications(&af_global_runtime);
        if (descriptors[1].revents & POLLIN)
          af_io_drain_completions(&af_global_runtime);
        pthread_mutex_lock(&af_global_runtime.mutex);
      } else if (af_global_runtime.timers != NULL)
        pthread_cond_timedwait(&af_global_runtime.ready,
                               &af_global_runtime.mutex,
                               &af_global_runtime.timers->deadline);
      else
        pthread_cond_wait(&af_global_runtime.ready, &af_global_runtime.mutex);
    }
    pthread_mutex_unlock(&af_global_runtime.mutex);

    pthread_mutex_lock(&task->mutex);
    if (task->cancel_requested) {
      task->status = AF_TASK_CANCELLED;
      pthread_mutex_unlock(&task->mutex);
      af_wake_waiters(task);
      continue;
    }
    if (task->status != AF_TASK_SCHEDULED) {
      pthread_mutex_unlock(&task->mutex);
      continue;
    }
    task->status = AF_TASK_RUNNING;
    pthread_mutex_unlock(&task->mutex);

    af_current_task = task;
    uint64_t result = af_invoke(task);
    af_current_task = NULL;

    pthread_mutex_lock(&task->mutex);
    enum af_task_status status = task->status;
    int cancelled = task->cancel_requested;
    pthread_mutex_unlock(&task->mutex);
    if (status == AF_TASK_RUNNING)
      af_finish(task, cancelled ? AF_TASK_CANCELLED : AF_TASK_COMPLETED,
                result);
  }
  return NULL;
}

static void af_runtime_init(void) {
  pthread_mutex_init(&af_global_runtime.mutex, NULL);
  pthread_condattr_t attr;
  pthread_condattr_init(&attr);
  pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
  pthread_cond_init(&af_global_runtime.ready, &attr);
  pthread_condattr_destroy(&attr);
  af_global_runtime.wake_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  if (af_global_runtime.wake_fd < 0) {
    fprintf(stderr,
            "AFlat async runtime: failed to create scheduler eventfd\n");
    abort();
  }
  af_io_ring_init(&af_global_runtime.io, 256);
  if (pthread_create(&af_global_runtime.worker, NULL, af_runtime_worker,
                     NULL) != 0) {
    fprintf(stderr, "AFlat async runtime: failed to start scheduler\n");
    abort();
  }
  pthread_detach(af_global_runtime.worker);
}

static af_runtime *af_runtime_get(void) {
  pthread_once(&af_runtime_once, af_runtime_init);
  return &af_global_runtime;
}

static af_task *af_task_create(void *resume, int argc, const uint64_t *args) {
  af_task *task = calloc(1, sizeof(*task));
  if (task == NULL)
    return NULL;
  pthread_mutex_init(&task->mutex, NULL);
  pthread_cond_init(&task->changed, NULL);
  task->resume = resume;
  task->argc = (unsigned char)argc;
  task->status = AF_TASK_CREATED;
  for (int i = 0; i < argc; ++i)
    task->args[i] = args[i];
  return task;
}

AF_ENTRY void *af_task_create0(void *resume) {
  return af_task_create(resume, 0, NULL);
}
AF_ENTRY void *af_task_create1(void *resume, uint64_t a0) {
  uint64_t args[] = {a0};
  return af_task_create(resume, 1, args);
}
AF_ENTRY void *af_task_create2(void *resume, uint64_t a0, uint64_t a1) {
  uint64_t args[] = {a0, a1};
  return af_task_create(resume, 2, args);
}
AF_ENTRY void *af_task_create3(void *resume, uint64_t a0, uint64_t a1,
                               uint64_t a2) {
  uint64_t args[] = {a0, a1, a2};
  return af_task_create(resume, 3, args);
}
AF_ENTRY void *af_task_create4(void *resume, uint64_t a0, uint64_t a1,
                               uint64_t a2, uint64_t a3) {
  uint64_t args[] = {a0, a1, a2, a3};
  return af_task_create(resume, 4, args);
}
AF_ENTRY void *af_task_create5(void *resume, uint64_t a0, uint64_t a1,
                               uint64_t a2, uint64_t a3, uint64_t a4) {
  uint64_t args[] = {a0, a1, a2, a3, a4};
  return af_task_create(resume, 5, args);
}
AF_ENTRY void *af_task_create6(void *resume, uint64_t a0, uint64_t a1,
                               uint64_t a2, uint64_t a3, uint64_t a4,
                               uint64_t a5) {
  uint64_t args[] = {a0, a1, a2, a3, a4, a5};
  return af_task_create(resume, 6, args);
}

AF_ENTRY void *af_task_spawn(void *opaque) {
  af_runtime_get();
  af_task *task = opaque;
  af_schedule(task);
  return task;
}

AF_ENTRY uint64_t af_task_run(void *opaque) {
  af_task *task = af_task_spawn(opaque);
  if (task == NULL)
    return 0;
  pthread_mutex_lock(&task->mutex);
  task->observed = 1;
  while (!af_terminal(task->status))
    pthread_cond_wait(&task->changed, &task->mutex);
  uint64_t result = task->result;
  enum af_task_status status = task->status;
  pthread_mutex_unlock(&task->mutex);
  if (status != AF_TASK_COMPLETED) {
    fprintf(stderr, "AFlat async runtime: root task did not complete\n");
    abort();
  }
  return result;
}

static void af_save_frame(af_task *task, int state, void *frame_base,
                          size_t frame_size) {
  if (task->frame_size != frame_size) {
    unsigned char *replacement = realloc(task->frame, frame_size);
    if (replacement == NULL && frame_size != 0) {
      fprintf(stderr, "AFlat async runtime: unable to allocate task frame\n");
      abort();
    }
    task->frame = replacement;
    task->frame_size = frame_size;
  }
  if (frame_size != 0)
    memcpy(task->frame, (unsigned char *)frame_base - frame_size, frame_size);
  task->state = state;
}

AF_ENTRY int af_task_restore_frame(void *frame_base, size_t frame_size) {
  af_task *task = af_current_task;
  if (task == NULL)
    return 0;
  pthread_mutex_lock(&task->mutex);
  int state = task->state;
  if (state != 0 && task->frame != NULL) {
    size_t count =
        task->frame_size < frame_size ? task->frame_size : frame_size;
    memcpy((unsigned char *)frame_base - frame_size, task->frame, count);
  }
  pthread_mutex_unlock(&task->mutex);
  return state;
}

AF_ENTRY void af_task_invalid_state(uint64_t state) {
  fprintf(stderr, "AFlat async runtime: invalid coroutine state %llu\n",
          (unsigned long long)state);
  if (af_current_task != NULL)
    af_finish(af_current_task, AF_TASK_FAILED, 0);
}

AF_ENTRY void af_task_await_suspend(void *opaque, int state, void *frame_base,
                                    size_t frame_size) {
  af_task *awaited = opaque;
  af_task *current = af_current_task;
  if (current == NULL || awaited == NULL || current == awaited) {
    fprintf(stderr, "AFlat async runtime: invalid self/null await\n");
    abort();
  }
  pthread_mutex_lock(&current->mutex);
  af_save_frame(current, state, frame_base, frame_size);
  current->awaited = awaited;
  current->status =
      current->cancel_requested ? AF_TASK_CANCELLED : AF_TASK_WAITING;
  int cancelled = current->status == AF_TASK_CANCELLED;
  pthread_mutex_unlock(&current->mutex);
  if (cancelled) {
    af_wake_waiters(current);
    return;
  }

  af_waiter *waiter = malloc(sizeof(*waiter));
  if (waiter == NULL) {
    af_finish(current, AF_TASK_FAILED, 0);
    return;
  }
  waiter->task = current;
  pthread_mutex_lock(&awaited->mutex);
  if (af_terminal(awaited->status)) {
    pthread_mutex_unlock(&awaited->mutex);
    free(waiter);
    af_schedule(current);
  } else {
    waiter->next = awaited->waiters;
    awaited->waiters = waiter;
    pthread_mutex_unlock(&awaited->mutex);
  }
  af_task_spawn(awaited);
}

AF_ENTRY uint64_t af_task_await_result(void) {
  af_task *current = af_current_task;
  if (current == NULL || current->awaited == NULL)
    return 0;
  af_task *awaited = current->awaited;
  pthread_mutex_lock(&awaited->mutex);
  enum af_task_status status = awaited->status;
  uint64_t result = awaited->result;
  awaited->observed = 1;
  pthread_mutex_unlock(&awaited->mutex);
  if (status != AF_TASK_COMPLETED) {
    fprintf(stderr, "AFlat async runtime: awaited task did not complete\n");
    abort();
  }
  return result;
}

AF_ENTRY void af_task_yield_suspend(int state, void *frame_base,
                                    size_t frame_size) {
  af_task *task = af_current_task;
  if (task == NULL)
    abort();
  pthread_mutex_lock(&task->mutex);
  af_save_frame(task, state, frame_base, frame_size);
  if (task->cancel_requested) {
    task->status = AF_TASK_CANCELLED;
    pthread_mutex_unlock(&task->mutex);
    af_wake_waiters(task);
    return;
  }
  task->status = AF_TASK_SCHEDULED;
  pthread_mutex_unlock(&task->mutex);
  pthread_mutex_lock(&af_global_runtime.mutex);
  af_enqueue_locked(&af_global_runtime, task);
  pthread_mutex_unlock(&af_global_runtime.mutex);
}

AF_ENTRY void af_task_pause_suspend(int state, void *frame_base,
                                    size_t frame_size) {
  af_task *task = af_current_task;
  if (task == NULL)
    abort();
  pthread_mutex_lock(&task->mutex);
  af_save_frame(task, state, frame_base, frame_size);
  if (task->cancel_requested) {
    task->status = AF_TASK_CANCELLED;
    pthread_mutex_unlock(&task->mutex);
    af_wake_waiters(task);
    return;
  }
  if (task->wake_pending) {
    task->wake_pending = 0;
    task->status = AF_TASK_SCHEDULED;
    pthread_mutex_unlock(&task->mutex);
    pthread_mutex_lock(&af_global_runtime.mutex);
    af_enqueue_locked(&af_global_runtime, task);
    pthread_mutex_unlock(&af_global_runtime.mutex);
    return;
  }
  task->status = AF_TASK_SUSPENDED;
  pthread_mutex_unlock(&task->mutex);
}

AF_ENTRY void *af_task_current_waker(void) { return af_current_task; }
AF_ENTRY void af_task_wake(void *opaque) { af_task_wake_internal(opaque); }

AF_ENTRY int af_task_status(void *opaque) {
  af_task *task = opaque;
  if (task == NULL)
    return AF_TASK_FAILED;
  pthread_mutex_lock(&task->mutex);
  int status = task->status;
  pthread_mutex_unlock(&task->mutex);
  return status;
}

AF_ENTRY void af_task_cancel(void *opaque) {
  af_task *task = opaque;
  if (task == NULL)
    return;
  pthread_mutex_lock(&task->mutex);
  task->cancel_requested = 1;
  af_io_operation *operation = task->pending_io;
  if (operation != NULL)
    atomic_store_explicit(&operation->abandoned, 1, memory_order_release);
  int release_operation =
      operation != NULL &&
      atomic_load_explicit(&operation->completed, memory_order_acquire);
  if (release_operation)
    task->pending_io = NULL;
  int idle = task->status == AF_TASK_CREATED ||
             task->status == AF_TASK_WAITING ||
             task->status == AF_TASK_SUSPENDED;
  if (idle)
    task->status = AF_TASK_CANCELLED;
  pthread_mutex_unlock(&task->mutex);
  if (release_operation)
    free(operation);
  if (idle)
    af_wake_waiters(task);
}

AF_ENTRY void af_task_detach(void *opaque) {
  af_task *task = opaque;
  if (task == NULL)
    return;
  pthread_mutex_lock(&task->mutex);
  task->detached = 1;
  pthread_mutex_unlock(&task->mutex);
  af_task_spawn(task);
}

AF_ENTRY int beginTimer(int milliseconds) {
  af_task *task = af_current_task;
  af_runtime *runtime = af_current_runtime;
  if (task == NULL || runtime == NULL)
    return -1;
  af_timer *timer = malloc(sizeof(*timer));
  if (timer == NULL)
    return -1;
  clock_gettime(CLOCK_MONOTONIC, &timer->deadline);
  timer->deadline.tv_sec += milliseconds / 1000;
  timer->deadline.tv_nsec += (milliseconds % 1000) * 1000000L;
  if (timer->deadline.tv_nsec >= 1000000000L) {
    timer->deadline.tv_sec += 1;
    timer->deadline.tv_nsec -= 1000000000L;
  }
  timer->task = task;
  pthread_mutex_lock(&runtime->mutex);
  af_timer **position = &runtime->timers;
  while (*position != NULL &&
         af_timespec_compare(&(*position)->deadline, &timer->deadline) <= 0)
    position = &(*position)->next;
  timer->next = *position;
  *position = timer;
  pthread_cond_signal(&runtime->ready);
  pthread_mutex_unlock(&runtime->mutex);
  return 0;
}

static int af_io_begin(int64_t descriptor, void *buffer, int64_t length,
                       int64_t offset, void **output, unsigned char opcode) {
  af_task *task = af_current_task;
  af_runtime *runtime = af_current_runtime;
  if (output != NULL)
    *output = NULL;
  if (task == NULL || runtime == NULL || output == NULL)
    return -EINVAL;

  af_io_operation *operation = calloc(1, sizeof(*operation));
  if (operation == NULL) {
    pthread_mutex_lock(&task->mutex);
    task->io_begin_error = -ENOMEM;
    pthread_mutex_unlock(&task->mutex);
    af_task_wake_internal(task);
    return -ENOMEM;
  }
  operation->task = task;
  operation->buffer = buffer;
  operation->opcode = opcode;
  *output = operation;

  pthread_mutex_lock(&task->mutex);
  if (task->pending_io != NULL) {
    task->io_begin_error = -EBUSY;
    pthread_mutex_unlock(&task->mutex);
    free(operation);
    *output = NULL;
    af_task_wake_internal(task);
    return -EBUSY;
  }
  task->io_begin_error = 0;
  task->pending_io = operation;
  pthread_mutex_unlock(&task->mutex);

  int error = buffer == NULL || length < 0
                  ? -EINVAL
                  : af_io_submit(runtime, operation, opcode, (int)descriptor,
                                 buffer, (size_t)length, offset);
  if (error != 0)
    af_io_complete(operation, error);
  return error;
}

AF_ENTRY int af_io_read_begin(int64_t descriptor, void *buffer, int64_t length,
                              int64_t offset, void **output) {
  return af_io_begin(descriptor, buffer, length, offset, output,
                     IORING_OP_READ);
}

AF_ENTRY int af_io_write_begin(int64_t descriptor, const void *buffer,
                               int64_t length, int64_t offset, void **output) {
  return af_io_begin(descriptor, (void *)buffer, length, offset, output,
                     IORING_OP_WRITE);
}

AF_ENTRY int64_t af_io_operation_result(void *opaque) {
  af_io_operation *operation = opaque;
  if (operation == NULL) {
    if (af_current_task != NULL) {
      pthread_mutex_lock(&af_current_task->mutex);
      int64_t error = af_current_task->io_begin_error;
      pthread_mutex_unlock(&af_current_task->mutex);
      if (error != 0)
        return error;
    }
    return -EINVAL;
  }
  if (!atomic_load_explicit(&operation->completed, memory_order_acquire))
    return -EINPROGRESS;
  return atomic_load_explicit(&operation->result, memory_order_relaxed);
}

AF_ENTRY void af_io_operation_release(void *opaque) {
  af_io_operation *operation = opaque;
  if (operation == NULL)
    return;
  af_task *task = operation->task;
  pthread_mutex_lock(&task->mutex);
  if (task->pending_io == operation)
    task->pending_io = NULL;
  pthread_mutex_unlock(&task->mutex);
  free(operation);
}
