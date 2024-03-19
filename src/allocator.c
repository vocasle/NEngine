#include "allocator.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

struct MemoryChunk
{
	const i8 *file;
	i32 line;
	u64 size;
};

#define MEMORY_GUARD 0xDEADF00D
#define MAX_MEMORY_ALLOCATED 1024 * 1024 * 1024 * 1024 // 1024 GB

void *ne_allocate_debug(u64 size, const i8 *file, i32 line)
{
	i8 *bytes = malloc(sizeof(struct MemoryChunk) + size * sizeof(i32) * 2);
	*((i32 *)bytes) = MEMORY_GUARD;
	*((i32 *)(bytes + sizeof(i32) + sizeof(struct MemoryChunk) + size)) = MEMORY_GUARD;
	struct MemoryChunk *mc = (struct MemoryChunk *)(bytes + sizeof(i32));
	memset(mc, 0, sizeof *mc + size);
	mc->file = file;
	mc->line = line;
	mc->size = size;
	void *ptr = mc + 1;
#if NE_VERBOSE_ALLOCATIONS
	printf("Allocating %llu bytes at %s:%d\n", size, file, line);
#endif
	return ptr;
}

void ne_free_debug(void *ptr, const i8 *file, i32 line)
{
	(void)file;
	(void)line;
	i8 *bytes = ptr;
	const i32 head_guard_value = *((i32*)(bytes - sizeof(struct MemoryChunk) - sizeof(i32)));
	assert((u32)head_guard_value == MEMORY_GUARD);
#if NE_VERBOSE_ALLOCATIONS
	if (head_guard_value != MEMORY_GUARD) {
		printf("ERROR: Memory corruption detected. Head guard corrupted. Expected: 0x%X, actual: 0x%X\n",
			MEMORY_GUARD, head_guard_value);
	}
#endif
	struct MemoryChunk *mc = ((struct MemoryChunk *)ptr) - 1;
	const b8 is_size_corrupted = mc->size > (u64)MAX_MEMORY_ALLOCATED;
	assert(!is_size_corrupted);
	const i32 tail_guard_value = *((i32*)(bytes + mc->size));
	assert((u32)tail_guard_value == MEMORY_GUARD);
#if NE_VERBOSE_ALLOCATIONS
	if (is_size_corrupted) {
		printf("ERROR: Memory corruption detected. Size corrupted. Expected memory less than 0x400 GB, actual: 0x%llX\n",
			mc->size);
	}
	if (tail_guard_value != MEMORY_GUARD) {
		printf("ERROR: Memory corruption detected. Tail guard corrupted. Expected: 0x%X, actual: 0x%X\n",
			MEMORY_GUARD, tail_guard_value);
	}
	printf("Deallocating %llu bytes at %s:%d, previously allocated at %s:%d\n", mc->size, file, line,
		mc->file, mc->line);
#endif
	ptr = NULL;
	bytes = bytes - sizeof(struct MemoryChunk) - sizeof(i32);
	free(bytes);
}

