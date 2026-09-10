#include "frame_allocator.h"
#define TOTAL_FRAMES 1024

static uint8_t frame_bitmap[TOTAL_FRAMES / 8];

uint32_t allocate_frame(void){
	for(uint32_t frame=0; frame<TOTAL_FRAMES; frame++){
		uint32_t byte = frame /8;
		uint32_t bit = frame % 8;

		if((frame_bitmap[byte] & (1 << bit)) ==0){
			frame_bitmap[byte] | = (1 << bit);
			return frame * FRAME_SIZE;
		}
	}
	return 0;
}

void free_frame(uint32_t frame_address){
	uint32_t frame = frame_address / FRAME_SIZE;

	uint32_t byte = frame / 8;
	uint32_t bit = frame % 8;

	frame_bitmap[byte] &= ~(1 << bit);  
}
