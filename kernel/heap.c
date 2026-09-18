#include "heap.h"
#include "paging.h"
#include "frame_allocator.h"

#define HEAP_START 0x02000000
#define PAGE_SIZE 4096
#define MAX_ALLOCATIONS 128

typedef struct{
	uint32_t start_address;
	uint32_t pages;
	uint8_t active;
} allocation_t;
static uint32_t heap_current = HEAP_START;
static allocation_t allocations[MAX_ALLOCATIONS];

void *kmalloc(uint32_t size){
	if(size == 0){
		return 0;
	}
	uint32_t pages = (size + PAGE_SIZE - 1)/ PAGE_SIZE;
	uint32_t start_address = heap_current;
	int allocation_index = -1;
	for(int i =0;i<MAX_ALLOCATIONS; i++){
		if(!allocations[i].active){
			allocation_index = i;
			break;
		}
	}
	if(allocation_index == -1){
		return 0;
	}
	for(uint32_t i =0; i<pages;i++){
		uint32_t frame = allocate_frame();
		if(frame == 0){
			return 0;
		}
		uint32_t virtual_address = heap_current + (i * PAGE_SIZE);
		if(!map_page(virtual_address, frame, PAGE_PRESENT | PAGE_WRITABLE)){
			free_frame(frame);
			return 0;
		}
	}
	
	allocations[allocation_index].start_address = start_address;
	allocations[allocation_index].pages = pages;
	allocations[allocation_index].active = 1;
	
	heap_current +=pages * PAGE_SIZE;
	return (void *)start_address;
}
void kfree(void *address){
	if(address == 0){
		return;
	}
	uint32_t target_address = (uint32_t)address;
	for(int i = 0; i<MAX_ALLOCATIONS; i++){
		if(allocations[i].active && allocations[i].start_address == target_address){
			for(uint32_t page =0; page<allocations[i].pages; page++){
				uint32_t virtual_address = target_address + (page * PAGE_SIZE);
 				unmap_page(virtual_address);

			}
			allocations[i].active =0;
			return;
		}
	}
}
