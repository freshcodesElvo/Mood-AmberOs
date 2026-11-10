#include "heap.h"
#include "paging.h"
#include "frame_allocator.h"

#define HEAP_START 0x02000000
#define PAGE_SIZE 4096
#define MAX_ALLOCATIONS 128
#define HEAP_END 0x03000000
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
	

	if(size > 0x0FFFFFFFFu-(PAGE_SIZE-1)){
		return 0;
	}
	uint32_t pages = (size + PAGE_SIZE - 1)/ PAGE_SIZE;

	//search for previously freed block of the same size
	/*for(int i = 0; i<MAX_ALLOCATIONS; i++){
		if(!allocations[i].active && allocations[i].start_address !=0 && allocations[i].pages == pages){
			uint32_t reused_address = allocations[i].start_address;
			uint32_t mapped_pages = 0;
			
			for(uint32_t page = 0; page<pages;page++){
				uint32_t frame = allocate_frame();
				if(frame==0){
					for(uint32_t j =0; j<mapped_pages; j++){
						unmap_page(reused_address+(j * PAGE_SIZE));
					}
					return 0;
				}
				uint32_t virtual_address = reused_address + (page * PAGE_SIZE);
				if(!map_page(virtual_address, frame, PAGE_PRESENT | PAGE_WRITABLE)){
					free_frame(frame);
					for(uint32_t j = 0; j<mapped_pages; j++){
						unmap_page(reused_address+(j*PAGE_SIZE));
					}
					return 0;
				}
				mapped_pages++;
			}
			allocations[i].active = 1;
			return (void *)reused_address;
		}
	}*/
	
	// Search for a suitable previously freed block.

int free_index = -1;
int remainder_index = -1;

// First: Look for an exact-size block.
for (int i = 0; i < MAX_ALLOCATIONS; i++)
{
    if (!allocations[i].active &&
        allocations[i].start_address != 0 &&
        allocations[i].pages == pages)
    {
        free_index = i;
        break;
    }
}

// Second: If no exact match exists, look for a larger block.
if (free_index == -1)
{
    for (int i = 0; i < MAX_ALLOCATIONS; i++)
    {
        if (!allocations[i].active &&
            allocations[i].start_address != 0 &&
            allocations[i].pages > pages)
        {
            // Find an unused allocation record
            for (int j = 0; j < MAX_ALLOCATIONS; j++)
            {
                if (!allocations[j].active &&
                    allocations[j].start_address == 0)
                {
                    free_index = i;
                    remainder_index = j;
                    break;
                }
            }

            if (free_index != -1)
            {
                break;
            }
        }
    }
}

// Reuse the selected block.
if (free_index != -1)
{
    uint32_t reused_address =
        allocations[free_index].start_address;

    uint32_t original_pages =
        allocations[free_index].pages;

    uint32_t mapped_pages = 0;

    for (uint32_t page = 0; page < pages; page++)
    {
        uint32_t frame = allocate_frame();

        if (frame == 0)
        {
            for (uint32_t j = 0; j < mapped_pages; j++)
            {
                unmap_page(reused_address + j * PAGE_SIZE);
            }

            return 0;
        }

        uint32_t virtual_address =
            reused_address + page * PAGE_SIZE;

        if (!map_page(
                virtual_address,
                frame,
                PAGE_PRESENT | PAGE_WRITABLE))
        {
            free_frame(frame);

            for (uint32_t j = 0; j < mapped_pages; j++)
            {
                unmap_page(reused_address + j * PAGE_SIZE);
            }

            return 0;
        }

        mapped_pages++;
    }

    // Update the allocation record.
    allocations[free_index].pages = pages;
    allocations[free_index].active = 1;

    // Preserve the unused remainder.
    if (remainder_index != -1)
    {
        allocations[remainder_index].start_address =
            reused_address + pages * PAGE_SIZE;

        allocations[remainder_index].pages =
            original_pages - pages;

        allocations[remainder_index].active = 0;
    }

    return (void *)reused_address;
}
	//2. allocate a new block if no free block exists

	//uint32_t start_address = heap_current;
	//uint32_t available_space = HEAP_END -  heap_current;
	if(heap_current>HEAP_END){
		return 0;
	}
	uint32_t available_space = HEAP_END - heap_current;
	if(pages>available_space/PAGE_SIZE){
		return 0;
	}
	uint32_t start_address = heap_current;
	int allocation_index = -1;
	for(int i =0;i<MAX_ALLOCATIONS; i++){
		if(!allocations[i].active &&  allocations[i].start_address ==0){
			allocation_index = i;
			break;
		}
	}
	if(allocation_index == -1){
		return 0;
	}
	uint32_t mapped_pages =0;
		
	for(uint32_t i =0; i<pages;i++){
		uint32_t frame = allocate_frame();
		if(frame == 0){
			for(uint32_t j = 0; j<mapped_pages; j++){
				unmap_page(start_address + (j * PAGE_SIZE));
			}
			return 0;
		}
		uint32_t virtual_address = heap_current + (i * PAGE_SIZE);
		if(!map_page(virtual_address, frame, PAGE_PRESENT | PAGE_WRITABLE)){
			free_frame(frame);
			for(uint32_t j =0; j<mapped_pages; j++){
				unmap_page(start_address + (j * PAGE_SIZE));
			}
			return 0;
		}
		mapped_pages++;
	}
	
	allocations[allocation_index].start_address = start_address;
	allocations[allocation_index].pages = pages;
	allocations[allocation_index].active = 1;
	
	heap_current +=pages * PAGE_SIZE;
	return (void *)start_address;
}


static void coalesce_free_blocks(void)
{
    int merged;

    do
    {
        merged = 0;

        for (int i = 0; i < MAX_ALLOCATIONS; i++)
        {
            if (allocations[i].active ||
                allocations[i].start_address == 0 ||
                allocations[i].pages == 0)
            {
                continue;
            }

            for (int j = i + 1; j < MAX_ALLOCATIONS; j++)
            {
                if (allocations[j].active ||
                    allocations[j].start_address == 0 ||
                    allocations[j].pages == 0)
                {
                    continue;
                }

                uint32_t end_i =
                    allocations[i].start_address +
                    allocations[i].pages * PAGE_SIZE;

                uint32_t end_j =
                    allocations[j].start_address +
                    allocations[j].pages * PAGE_SIZE;

                // Block j comes immediately after block i.
                if (end_i == allocations[j].start_address)
                {
                    allocations[i].pages += allocations[j].pages;

                    allocations[j].start_address = 0;
                    allocations[j].pages = 0;

                    merged = 1;
                    break;
                }

                // Block j comes immediately before block i.
                else if (end_j == allocations[i].start_address)
                {
                    allocations[i].start_address =
                        allocations[j].start_address;

                    allocations[i].pages += allocations[j].pages;

                    allocations[j].start_address = 0;
                    allocations[j].pages = 0;

                    merged = 1;
                    break;
                }
            }

            if (merged)
            {
                break;
            }
        }

    } while (merged);
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
			            coalesce_free_blocks();

			return;
		}
	}
}
