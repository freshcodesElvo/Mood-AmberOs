#include "paging.h"
#include "frame_allocator.h"
#include <stdint.h>
#include "screen.h"

extern uint32_t kernel_start;
extern uint32_t kernel_end;
#define PAGE_SIZE 4096
#define PAGE_ENTRIES 1024

static uint32_t *page_directory;

static uint32_t *first_page_table;
static uint8_t page_table_dynamic[PAGE_ENTRIES];

void paging_init(void)
{
    uint32_t page_directory_address = allocate_frame();
    uint32_t first_page_table_address = allocate_frame();
    

    page_directory = (uint32_t *)page_directory_address;
    first_page_table = (uint32_t *)first_page_table_address;
    

    print("Page directory allocated at: ");
    print_hex(page_directory_address);
    print("\n");

    print("First page table allocated at: ");
    print_hex(first_page_table_address);
    print("\n");

    



    // Clear the page directory
    for (int i = 0; i < PAGE_ENTRIES; i++)
    {
        page_directory[i] = 0;
    }

    // Identity-map the first 4 MB
    for (int i = 0; i < PAGE_ENTRIES; i++)
    {
        first_page_table[i] = (i * PAGE_SIZE) | 3;
    }

    // Connect page tables to the page directory
    page_directory[0] = ((uint32_t)first_page_table) | 3;
   

    // Load page directory into CR3
    __asm__ volatile (
        "mov %0, %%cr3"
        :
        : "r"(page_directory)
    );

    // Enable paging
    uint32_t cr0;

    __asm__ volatile (
        "mov %%cr0, %0"
        : "=r"(cr0)
    );

    cr0 |= 0x80000000;

    __asm__ volatile (
        "mov %0, %%cr0"
        :
        : "r"(cr0)
    );
}









void map_page(uint32_t virtual_address, uint32_t physical_address){
	uint32_t kernel_start_address = (uint32_t)&kernel_start;
	uint32_t kernel_end_address =(uint32_t)&kernel_end;
	

	if(virtual_address>=kernel_start_address && virtual_address < kernel_end_address){
		print("Error:>> Cannot map over kernel memory!\n");
		return;
	}
	uint32_t directory_index = (virtual_address >> 22) & 0x3FF;
	uint32_t table_index = (virtual_address >> 12) & 0x3FF;

	//check whether a pge table already exists
	if((page_directory[directory_index] & 1)==0){
		create_page_table(directory_index);
	
	}

	//get the pge table address from the pge directory entry
	uint32_t page_table_address = page_directory[directory_index]& 0xFFFFF000;
	uint32_t *page_table = (uint32_t *)page_table_address;

	//map the virtual page to the physical frame
	//0x3 = present + writable
	page_table[table_index] = (physical_address & 0xFFFFF000) | 3;

	//tell the cpu to invalidate this virtual addr from the TLB

__asm__ volatile(
	"invlpg (%0)"
	:
	: "r"(virtual_address)
	: "memory"	
		);
}
uint32_t create_page_table(uint32_t directory_index){
	uint32_t page_table_address = allocate_frame();
	if(page_table_address ==0){
		print("Error: Could not allocate page table!\n");
		while(1){
			__asm__ volatile ("hlt");
		}
	}

	uint32_t *page_table = (uint32_t *)page_table_address;
	//clear all 1024 page table entries
	for(int i = 0; i< PAGE_ENTRIES; i++){
		page_table[i]=0;
	}
	//connect the pge table to the pge directory
	//0x3 = present + writable
	
	page_directory[directory_index] = page_table_address | 3;
	page_table_dynamic[directory_index] = 1;

	return page_table_address;
}
void unmap_page(uint32_t virtual_address){
	uint32_t directory_index = (virtual_address >> 22) & 0x3FF;
	uint32_t table_index = (virtual_address >> 12) & 0x3FF;
	//check whether the page table exists
	if((page_directory[directory_index]&1)==0){
		print("Error: The  page table does not exist!\n");
		return;
	}
	//get the page table address
	uint32_t page_table_address=page_directory[directory_index] & 0xFFFFF000;

	uint32_t *page_table = (uint32_t *)page_table_address;

	//check whether the pge itself is mapped
	if((page_table[table_index]& 1)==0){
		print("Error: Page not mapped!!\n");
		return;
	}
	//get the physical frame before unmapping
	uint32_t physical_address = page_table[table_index] & 0xFFFFF000;
	//unmap
	page_table[table_index] = 0;
	//tell cpu to remove the old translation from its TLB
	__asm__ volatile(
		"invlpg (%0)"
		:
		: "r"(virtual_address)
		: "memory"
	);
	//retyrn the physical frame to the frame allocator
	free_frame(physical_address);
	int table_empty = 1;
	for(int i=0; i<PAGE_ENTRIES;i++){
		if(page_table[i] & 1){
			table_empty = 0;
			break;
		}
	}

	if(table_empty && page_table_dynamic[directory_index]){
		page_directory[directory_index] = 0;
		page_table_dynamic[directory_index] = 0;
		free_frame(page_table_address);
	}
}
