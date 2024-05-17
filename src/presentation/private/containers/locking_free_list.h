#pragma once

#include <shared/std.h>
#include <shared/slice.h>
#include <shared/allocator.h>
#include <shared/log.h>

#include <platform/platform.h>

template <typename T>
struct LockingFreeList
{

	usize calcNodeIndex(void* node);

	void init(usize size, Allocator* in_allocator, Platform* in_platform)
	{
		platform = in_platform;
		mutex = in_platform->create_mutex();
		free_slot_count = size;
		allocator = in_allocator;
		nodes = in_allocator->alloc<Node>(size);
		for (usize i = 0; i < size - 1; i++)
		{
			nodes[i].next = i + 1;
			// nodes[i].index = i;
		}
		head = 0;
	}

	void deinit()
	{
		allocator->free(nodes);
		platform->destroy_mutex(mutex);
	}

	T* alloc()
	{
		platform->lock_mutex(mutex);
		PAW_ASSERT(free_slot_count > 0);
		// PAW_LOG_INFO("Head: %llu - This: %llu", head, (u64)this);
		usize free_slot_index = head;
		Node* free_slot = &nodes[free_slot_index];
		head = free_slot->next;
		free_slot_count--;

		platform->unlock_mutex(mutex);
		return reinterpret_cast<T*>(free_slot);
	}

	void free(T* item)
	{
		Node* node = reinterpret_cast<Node*>(item);
		platform->lock_mutex(mutex);
		usize index = calcNodeIndex(node);
		node->next = head;
		head = index;
		free_slot_count++;
		platform->unlock_mutex(mutex);
	}

	union Node
	{
		T data;
		usize next;
	};

	Allocator* allocator;
	Slice<Node> nodes;
	usize head;
	usize free_slot_count;
	PlatformMutex mutex;
	Platform* platform;
};

template <typename T>
usize LockingFreeList<T>::calcNodeIndex(void* node)
{
	const u64 n = (u64)node;
	const u64 start = (u64)nodes.ptr;
	return (n - start) / sizeof(Node);
}