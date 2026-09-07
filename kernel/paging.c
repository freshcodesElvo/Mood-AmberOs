#include "paging.h"
#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 1024

static uint32_t page_directory[PAGE_ENTRIES]
    __attribute__((aligned(PAGE_SIZE)));

static uint32_t first_page_table[PAGE_ENTRIES]
    __attribute__((aligned(PAGE_SIZE)));

void paging_init(void)
{
    /*
     * Clear the page directory.
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
    page_directory[0] =
        ((uint32_t)first_page_table) | 3;

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
