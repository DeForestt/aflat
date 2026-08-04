#define _POSIX_C_SOURCE 200809L

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
};

struct af_timer {
  struct timespec deadline;
  af_task *task;
  af_timer *next;
};

typedef struct af_runtime {
  pthread_mutex_t mutex;
  pthread_cond_t ready;
  pthread_t worker;
  af_task *queue_head;
  af_task *queue_tail;
  af_timer *timers;
} af_runtime;

static af_runtime af_global_runtime;
static pthread_once_t af_runtime_once = PTHREAD_ONCE_INIT;
static _Thread_local af_runtime *af_current_runtime;
static _Thread_local af_task *af_current_task;

#if defined(__x86_64__)
#define AF_ENTRY __attribute__((force_align_arg_pointer))
#else
#define AF_ENTRY
#endif

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

static void af_enqueue_locked(af_runtime *runtime, af_task *task) {
  task->queue_next = NULL;
  if (runtime->queue_tail == NULL)
    runtime->queue_head = runtime->queue_tail = task;
  else {
    runtime->queue_tail->queue_next = task;
    runtime->queue_tail = task;
  }
  pthread_cond_signal(&runtime->ready);
}

static void af_schedule(af_task *task) {
  if (task == NULL)
    return;
  pthread_mutex_lock(&task->mutex);
  if (af_terminal(task->status) || task->status == AF_TASK_SCHEDULED ||
      task->status == AF_TASK_RUNNING) {
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
    return ((uint64_t(*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t))
                task->resume)(task->args[0], task->args[1], task->args[2],
                              task->args[3], task->args[4]);
  default:
    return ((uint64_t(*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t,
                         uint64_t))task->resume)(
        task->args[0], task->args[1], task->args[2], task->args[3],
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
      if (af_global_runtime.timers != NULL)
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
  if (pthread_create(&af_global_runtime.worker, NULL, af_runtime_worker, NULL) !=
      0) {
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
    size_t count = task->frame_size < frame_size ? task->frame_size : frame_size;
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
  current->status = current->cancel_requested ? AF_TASK_CANCELLED
                                              : AF_TASK_WAITING;
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
  int idle = task->status == AF_TASK_CREATED ||
             task->status == AF_TASK_WAITING ||
             task->status == AF_TASK_SUSPENDED;
  if (idle)
    task->status = AF_TASK_CANCELLED;
  pthread_mutex_unlock(&task->mutex);
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
