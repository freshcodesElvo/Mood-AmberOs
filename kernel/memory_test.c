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

	map_page(0x01000000, frame, PAGE_PRESENT | PAGE_WRITABLE);
	volatile uint32_t *address = (uint32_t *)0x01000000;
	*address = 0x12345678;

	print("Testing duplicate mapping\n");
        map_page(0x01000000, frame, PAGE_PRESENT | PAGE_WRITABLE);
        print("Duplicate mapping test finished\n");

		
	print("Mapped and wrote succesfully!\n");
	uint32_t value = *(volatile uint32_t *)frame;

	print("Read through identity mapping: ");
	print_hex(value);
	print("\n");

	
	unmap_page(0x01000000);
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

	map_page((uint32_t)&kernel_start, protection_frame, PAGE_PRESENT | PAGE_WRITABLE);
	free_frame(protection_frame);

	print("\nTESTING READ ONLY PAGEING PROTECTION\n");
	uint32_t readonly_frame = allocate_frame();
	print("Read only frame: ");
	print_hex(readonly_frame);
	print("\n");

	map_page(0x01400000, readonly_frame, PAGE_PRESENT);
	print("Read only page mapped!!\n");
	print("Attempting to write....\n");
	volatile uint32_t *readonly_address = (uint32_t *)0x01400000;
	//\*readonly_address = 0xDEADBEEF;
	print("Write test skipped (read-only page).\n");
	print("Testing duplicate mapping\n");
	map_page(0x01000000, frame, PAGE_PRESENT | PAGE_WRITABLE);
	
	
	print("Testing page table reclamation\n");
	uint32_t reclaim_test_frame =  allocate_frame();
	map_page(0x01800000, reclaim_test_frame, PAGE_PRESENT | PAGE_WRITABLE);
	print("Temporary page mapped\n");
	unmap_page(0x01800000);
	print("Temporary page unmapped\n");
	
	print("Testing remap after reclamation\n");
	uint32_t remap_frame = allocate_frame();
	map_page(0x01800000, remap_frame, PAGE_PRESENT | PAGE_WRITABLE);
	volatile uint32_t *remap_address = (uint32_t *)0x01800000;
	*remap_address = 0xCAFEBABE;
	print("Remapped succesfully: >>");
	print_hex(*remap_address);
	print("\n");
	unmap_page(0x01800000);
	print("Remap test completed\n");	
}

