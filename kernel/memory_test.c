#include "screen.h"
#include "frame_allocator.h"
#include  "paging.h"

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
}
