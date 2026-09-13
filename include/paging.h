#ifndef PAGING_H
#define PAGING_H
#include <stdint.h>
extern uint32_t test_frame_address;
void paging_init(void);
void map_page(uint32_t virtual_address, uint32_t physical_address);
void unmap_page(uint32_t virtual_address);
uint32_t create_page_table(uint32_t directory_index);
#endif
