#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

struct multiboot_mmap_entry{
	uint32_t size;
	uint64_t addr;
	uint64_t len;
	uint32_t type;

 }__attribute__((packed));

#endif
