#ifndef PAGING_H
#define PAGING_H

#include  <stdint.h>
#define PAGE_PRESENT 0x001
#define PAGE_WRITABLE 0x002
#define PAGE_USER 0x004


void paging_init(void);
int map_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags);
void unmap_page(uint32_t virtual_address);
uint32_t create_page_table(uint32_t directory_index);
#endif
