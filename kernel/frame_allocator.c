#include "frame_allocator.h"
#include "multiboot.h"
#include "screen.h"
#define MAX_FRAMES 1048576

static uint8_t frame_bitmap[MAX_FRAMES/8];
extern uint32_t kernel_start;
extern uint32_t kernel_end;

static void set_frame(uint32_t frame){
	frame_bitmap[frame/8] |= (1<<(frame%8));
	
}
static void clear_frame(uint32_t frame){
	frame_bitmap[frame/8] &= ~(1<<(frame%8));
}
static int is_frame_used(uint32_t frame){
	return frame_bitmap[frame/8]&(1<<(frame%8));
}
void frame_allocator_init(uint32_t multiboot_info_addr){
	/*SAFEST DEFAULT: EVERY FRAME STARTS AS USED*/

	for(uint32_t i = 0; i<MAX_FRAMES/8; i++){
		frame_bitmap[i] = 0xFF;
	}
	uint32_t mmap_addr = *(uint32_t *)(multiboot_info_addr + 48);
	uint32_t mmap_len = *(uint32_t *)(multiboot_info_addr + 44);
	uint32_t mmap_end = mmap_addr + mmap_len;
	
	struct multiboot_mmap_entry *entry = (struct multiboot_mmap_entry *)mmap_addr;

	/*MARK MULTIBOOT TYPE 1 AS FREE*/
	while((uint32_t)entry<mmap_end){
	
		if(entry->size < 20){
			break;
		}
		if(entry->type == 1 && entry->addr>=0x00100000){
			uint64_t start = entry->addr;
			uint64_t length = entry->len;
			uint64_t end = start + length;

			uint32_t first_frame = (uint32_t)((start + FRAME_SIZE-1)/FRAME_SIZE);
			uint32_t last_frame = (uint32_t)(end/FRAME_SIZE);
			for(uint32_t frame = first_frame; frame<last_frame;frame++){
				clear_frame(frame);
			}
		}
		entry = (struct multiboot_mmap_entry *)((uint8_t *)entry+entry->size+4);
	
	}
	/*RESERVE EVERYTHING OCCUPIED BY THE KERNEL*/
	uint32_t kernel_first = (uint32_t)&kernel_start/FRAME_SIZE;
	uint32_t kernel_last = ((uint32_t)&kernel_end + FRAME_SIZE-1)/FRAME_SIZE;
	for(uint32_t frame = kernel_first; frame<kernel_last; frame++){
		set_frame(frame);
	}
	
}

uint32_t allocate_frame(void){
	for(uint32_t frame = 0; frame < MAX_FRAMES; frame++){
		if(!is_frame_used(frame)){
			set_frame(frame);
			return frame * FRAME_SIZE;
		}
	}
	return 0;
}

void free_frame(uint32_t frame_address)
{

    uint32_t frame = frame_address / FRAME_SIZE;


    if (frame < MAX_FRAMES)
    {
        clear_frame(frame);
   
    }
}
