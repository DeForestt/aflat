#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/*
 * AFlat's public allocator ABI is backed by libc allocations. An independent
 * active-allocation registry preserves the old allocator's tolerance for null,
 * duplicate, and interior-pointer frees without scanning heap history.
 */

#define AF_ALLOCATOR_INITIAL_BUCKETS 1024u
#define AF_ALLOCATOR_MAGIC UINT64_C(0x41464c41544d454d)

typedef union af_allocation_header af_allocation_header;

union af_allocation_header {
  max_align_t alignment;
  struct {
    uint64_t magic;
    size_t size;
    af_allocation_header *hash_next;
    af_allocation_header *active_previous;
    af_allocation_header *active_next;
  } fields;
};

static pthread_mutex_t af_registry_mutex = PTHREAD_MUTEX_INITIALIZER;
static af_allocation_header **af_registry_buckets;
static size_t af_registry_bucket_count;
static af_allocation_header *af_active_head;
static int af_live_block_count;
static long af_live_byte_count;
static long af_allocation_count;

static void *af_header_payload(af_allocation_header *header) {
  return (void *)(header + 1);
}

static size_t af_pointer_hash(const void *pointer, size_t bucket_count) {
  uintptr_t value = (uintptr_t)pointer;
  value >>= 4;
  value ^= value >> 33;
  value *= UINT64_C(0xff51afd7ed558ccd);
  value ^= value >> 33;
  return (size_t)value & (bucket_count - 1);
}

static int af_registry_initialize(void) {
  if (af_registry_buckets != NULL)
    return 1;

  af_registry_buckets =
      calloc(AF_ALLOCATOR_INITIAL_BUCKETS, sizeof(*af_registry_buckets));
  if (af_registry_buckets == NULL)
    return 0;
  af_registry_bucket_count = AF_ALLOCATOR_INITIAL_BUCKETS;
  return 1;
}

static af_allocation_header *
af_registry_find(const void *pointer, size_t *bucket_index) {
  if (af_registry_buckets == NULL || pointer == NULL)
    return NULL;

  const size_t index =
      af_pointer_hash(pointer, af_registry_bucket_count);
  if (bucket_index != NULL)
    *bucket_index = index;

  af_allocation_header *current = af_registry_buckets[index];
  while (current != NULL) {
    if (af_header_payload(current) == pointer)
      return current;
    current = current->fields.hash_next;
  }
  return NULL;
}

static void af_registry_rehash(size_t new_bucket_count) {
  af_allocation_header **replacement =
      calloc(new_bucket_count, sizeof(*replacement));
  if (replacement == NULL)
    return;

  af_allocation_header *current = af_active_head;
  while (current != NULL) {
    const size_t index =
        af_pointer_hash(af_header_payload(current), new_bucket_count);
    current->fields.hash_next = replacement[index];
    replacement[index] = current;
    current = current->fields.active_next;
  }

  free(af_registry_buckets);
  af_registry_buckets = replacement;
  af_registry_bucket_count = new_bucket_count;
}

static int af_registry_insert(af_allocation_header *header) {
  if (!af_registry_initialize())
    return 0;

  if ((size_t)(af_live_block_count + 1) >
      (af_registry_bucket_count * 3) / 4)
    af_registry_rehash(af_registry_bucket_count * 2);

  const size_t index =
      af_pointer_hash(af_header_payload(header), af_registry_bucket_count);
  header->fields.hash_next = af_registry_buckets[index];
  af_registry_buckets[index] = header;

  header->fields.active_previous = NULL;
  header->fields.active_next = af_active_head;
  if (af_active_head != NULL)
    af_active_head->fields.active_previous = header;
  af_active_head = header;
  return 1;
}

static void af_registry_remove(af_allocation_header *header,
                               size_t bucket_index) {
  af_allocation_header **link = &af_registry_buckets[bucket_index];
  while (*link != NULL && *link != header)
    link = &(*link)->fields.hash_next;
  if (*link == header)
    *link = header->fields.hash_next;

  if (header->fields.active_previous != NULL)
    header->fields.active_previous->fields.active_next =
        header->fields.active_next;
  else
    af_active_head = header->fields.active_next;

  if (header->fields.active_next != NULL)
    header->fields.active_next->fields.active_previous =
        header->fields.active_previous;
}

void *af_malloc(int size) {
  if (size < 0)
    return NULL;

  const size_t requested = (size_t)size;
  if (requested > SIZE_MAX - sizeof(af_allocation_header))
    return NULL;

  const size_t payload_size = requested == 0 ? 1 : requested;
  af_allocation_header *header =
      malloc(sizeof(*header) + payload_size);
  if (header == NULL)
    return NULL;

  header->fields.magic = AF_ALLOCATOR_MAGIC;
  header->fields.size = requested;

  pthread_mutex_lock(&af_registry_mutex);
  if (!af_registry_insert(header)) {
    pthread_mutex_unlock(&af_registry_mutex);
    free(header);
    return NULL;
  }
  ++af_live_block_count;
  af_live_byte_count += (long)requested;
  ++af_allocation_count;
  pthread_mutex_unlock(&af_registry_mutex);

  return af_header_payload(header);
}

int af_free(void *pointer) {
  if (pointer == NULL)
    return 0;

  pthread_mutex_lock(&af_registry_mutex);
  size_t bucket_index = 0;
  af_allocation_header *header =
      af_registry_find(pointer, &bucket_index);
  if (header == NULL || header->fields.magic != AF_ALLOCATOR_MAGIC) {
    pthread_mutex_unlock(&af_registry_mutex);
    return 0;
  }

  af_registry_remove(header, bucket_index);
  --af_live_block_count;
  af_live_byte_count -= (long)header->fields.size;
  header->fields.magic = 0;
  pthread_mutex_unlock(&af_registry_mutex);

  free(header);
  return 0;
}

void *af_realloc(void *pointer, int size) {
  if (pointer == NULL)
    return af_malloc(size);
  if (size < 0)
    return NULL;

  pthread_mutex_lock(&af_registry_mutex);
  af_allocation_header *header = af_registry_find(pointer, NULL);
  if (header == NULL || header->fields.magic != AF_ALLOCATOR_MAGIC) {
    pthread_mutex_unlock(&af_registry_mutex);
    return NULL;
  }
  const size_t previous_size = header->fields.size;
  if (previous_size >= (size_t)size) {
    pthread_mutex_unlock(&af_registry_mutex);
    return pointer;
  }
  pthread_mutex_unlock(&af_registry_mutex);

  void *replacement = af_malloc(size);
  if (replacement == NULL)
    return NULL;
  memcpy(replacement, pointer, previous_size);
  af_free(pointer);
  return replacement;
}

int blockSize(void *pointer) {
  if (pointer == NULL)
    return 0;

  pthread_mutex_lock(&af_registry_mutex);
  af_allocation_header *header = af_registry_find(pointer, NULL);
  const int size =
      header != NULL && header->fields.magic == AF_ALLOCATOR_MAGIC
          ? (int)header->fields.size
          : 0;
  pthread_mutex_unlock(&af_registry_mutex);
  return size;
}

int af_live_blocks(void) {
  pthread_mutex_lock(&af_registry_mutex);
  const int count = af_live_block_count;
  pthread_mutex_unlock(&af_registry_mutex);
  return count;
}

long af_live_bytes(void) {
  pthread_mutex_lock(&af_registry_mutex);
  const long count = af_live_byte_count;
  pthread_mutex_unlock(&af_registry_mutex);
  return count;
}

long af_total_allocations(void) {
  pthread_mutex_lock(&af_registry_mutex);
  const long count = af_allocation_count;
  pthread_mutex_unlock(&af_registry_mutex);
  return count;
}

int inspectHeap(void) {
  pthread_mutex_lock(&af_registry_mutex);
  printf("Heap: live blocks=%d live bytes=%ld total allocations=%ld\n",
         af_live_block_count, af_live_byte_count, af_allocation_count);
  printf("HEAD --> ");
  af_allocation_header *current = af_active_head;
  while (current != NULL) {
    printf("%p: status: 0 size: %zu overhead: %zu\n         ",
           af_header_payload(current), current->fields.size,
           sizeof(*current));
    current = current->fields.active_next;
  }
  printf("NULL\n");
  pthread_mutex_unlock(&af_registry_mutex);
  return 0;
}
