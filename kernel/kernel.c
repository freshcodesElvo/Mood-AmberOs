#include  <stdint.h>
#include "screen.h"
#include "interrupts.h"
#include "idt.h"
#include "paging.h"
#include "multiboot.h"
#include "frame_allocator.h"
#include "memory_test.h"
//#include "frame_allocator_test.h"

extern uint32_t kernel_start;
extern uint32_t kernel_end;

void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_info_addr)
{
    clear_screen();

    print("---------------------------------\n");
    print("MOOD-AMBER OS\n");
    print("---------------------------------\n\n");
	
	print("Multiboot magic >>>> ");
	print_hex(multiboot_magic);
	print("\n");

	



	

	/* FRAME ALLOCATOR */

	print("\nStarting frame allocator...\n");

	frame_allocator_init(multiboot_info_addr);
	//frame_allocator_test();
	print("Frame allocator initialized!\n");



		
    print("Kernel Version 0.0.1\n");

    interrupts_init();
    idt_init();

	paging_init();

	print("\n\n******** RUNNING NEW MEMORY TEST ********\n");
	memory_test();
	print("******** NEW MEMORY TEST FINISHED ********\n");	

	
    while (1)
    {
    }
}
