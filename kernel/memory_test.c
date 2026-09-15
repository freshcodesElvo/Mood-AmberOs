#include "screen.h"
#include "frame_allocator.h"
#include  "paging.h"
extern uint32_t kernel_start;

void memory_test(void){
	print("\n              MEMORY MANAGEMENT TEST                  \n");

	uint32_t frame = allocate_frame();
	
	print("Allocated frame: ");
	print_hex(frame);
	print("\n");

	map_page(0x01000000, frame);
	volatile uint32_t *address = (uint32_t *)0x01000000;
	*address = 0x12345678;
	
	print("Mapped and wrote succesfully!\n");
	uint32_t value = *(volatile uint32_t *)frame;

	print("Read through identity mapping: ");
	print_hex(value);
	print("\n");

	print("Page unmapped\n");
	uint32_t recycled = allocate_frame();
	print("Recycled frame: ");
	print_hex(recycled);
	print("\n");


	print("\n TESTING KERNEL MEMORY PROTECTION\n");
	uint32_t protection_frame = allocate_frame();
	print("Protection test frame: ");
	print_hex(protection_frame);
	print("\n");

	map_page((uint32_t)&kernel_start, protection_frame);
	free_frame(protection_frame);
}
