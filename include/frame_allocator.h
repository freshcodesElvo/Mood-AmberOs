#ifndef FRAME_ALLOCATOR_H
#define FRAME_ALLOCATOR_H

#include <stdint.h>

#define FRAME_SIZE 4096
void frame_allocator_init(void);

uint32_t allocate_frame(void);
void free_frame(uint32_t frame_address);

#endif
