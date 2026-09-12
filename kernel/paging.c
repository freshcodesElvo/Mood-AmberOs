
#include "paging.h"
#include "frame_allocator.h"
#include <stdint.h>
#include "screen.h"

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 1024

static uint32_t *page_directory;

static uint32_t *first_page_table;
static uint32_t *second_page_table;

uint32_t test_frame_address;

void paging_init(void)
{
	
	uint32_t page_directory_address = allocate_frame();
	uint32_t first_page_table_address = allocate_frame();
	uint32_t second_page_table_address = allocate_frame();

	page_directory = (uint32_t *)page_directory_address;
	first_page_table = (uint32_t *)first_page_table_address;
	second_page_table = (uint32_t *)second_page_table_address;
	
	

	print("Page directory allocated at: ");
	print_hex(page_directory_address);
	print("\n");

	print("First page table allocated at: ");
	print_hex(first_page_table_address);
	print("\n");

	print("2nd page table allocated at: ");
        print_hex(second_page_table_address);
        print("\n");

	test_frame_address = allocate_frame();
	print("test page allocated at: >>>>>");
	print_hex(test_frame_address);
	print("\n");
	second_page_table[0]=test_frame_address | 3;

	//Clear the second page table
	for(int i = 0; i< PAGE_ENTRIES; i++){
		second_page_table[i] = 0;
	}

	//Map virtual address 0x00400000 to our allocated physical frame

	second_page_table[0] = test_frame_address |3;
	 /* Clear the page directory.
     */
    for (int i = 0; i < PAGE_ENTRIES; i++)
    {
        page_directory[i] = 0;
    }

    /*
     * Identity-map the first 4 MB of memory.
     *
     * Virtual address = Physical address
     */
    for (int i = 0; i < PAGE_ENTRIES; i++)
    {
        first_page_table[i] = (i * PAGE_SIZE) | 3;
    }

    /*
     * Page directory entry 0 points to our first page table.
     *
     * 0x3 = Present + Writable
     */
    page_directory[0] =  ((uint32_t)first_page_table) | 3;
    page_directory[1] =((uint32_t)second_page_table)|3;

    /*
     * Load the page directory into CR3.
     */
    __asm__ volatile (
        "mov %0, %%cr3"
        :
        : "r"(page_directory)
    );

    /*
     * Enable paging by setting bit 31 of CR0.
     */
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
