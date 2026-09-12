#include  <stdint.h>
#include "screen.h"
#include "interrupts.h"
#include "idt.h"
#include "paging.h"
#include "multiboot.h"
#include "frame_allocator.h"

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

	
	print("First word at MBI address : ");
	print_hex(*(uint32_t *)multiboot_info_addr);
	print("\n");

	print("Second word at MBI addr: ");
	print_hex(*(uint32_t *)(multiboot_info_addr + 4));
	print("\n");
        print("multiboot information address: ");
   	print_hex(multiboot_info_addr);
	print("\n");
	
	print("Memory lower: ");
	print_hex(*(uint32_t *)(multiboot_info_addr +4));
	print("\n");

	print("Memory upper:");
	print_hex(*(uint32_t *)(multiboot_info_addr + 8));
	print("\n");

	print("Memory map length: ");
	print_hex(*(uint32_t *)(multiboot_info_addr + 44));
	print("\n");

	print("Memory map address: ");
	print_hex(*(uint32_t *)(multiboot_info_addr +48));
	print("\n");


	print("\n Kenrnel bounderies: \n");
	print("Kernel start: ");
	print_hex((uint32_t)&kernel_start);
	print("\n");

	print("kernel end: ");
	print_hex((uint32_t)&kernel_end);
	print("\n");
	
	



	if(multiboot_magic != 0x2BADB002)
{
    print("Invalid multiboot magic!!!!");

    while(1)
    {
        __asm__ volatile ("hlt");
    }
}


	/* FRAME ALLOCATOR */

	print("\nStarting frame allocator...\n");

	frame_allocator_init(multiboot_info_addr);

	print("Frame allocator initialized!\n");

	uint32_t frame1 = allocate_frame();
	uint32_t frame2 = allocate_frame();
	uint32_t frame3 = allocate_frame();

	print("Allocated frames:\n");

	print("Frame 1 = ");
	print_hex(frame1);
	print("\n");

	print("Frame 2 = ");
	print_hex(frame2);

	print("Frame 3 = ");
	print_hex(frame3);

	print("\nFREE FRAME TEST\n");

	print("Before free_frame\n");

	free_frame(frame2);

	print("After free_frame\n");

	uint32_t frame4 = allocate_frame();

	print("After allocate_frame\n");

	print("Frame 4 = ");
	print_hex(frame4);
	print("\n");

	/* MEMORY MAP */
	print("\nMemory Map:\n");

	struct multiboot_mmap_entry *entry = 
		(struct multiboot_mmap_entry *)
		(*(uint32_t *)(multiboot_info_addr + 48));

	uint32_t mmap_end = 
		(*(uint32_t *)(multiboot_info_addr + 48))+
		(*(uint32_t *)(multiboot_info_addr + 44));

	print("\nMemory Map:\n");

	while((uint32_t)entry< mmap_end){

		if(entry->size < 20){
			break;
		}
		print("Size: ");
		print_hex(entry->size);
		
		print("	Start");
		print_hex((uint32_t)entry->addr);

		print("	Length: ");
		print_hex((uint32_t)entry->len);

		print("	Type");
		print_hex(entry->type);

		print("\n");

		entry = (struct multiboot_mmap_entry *)
			((uint8_t *)entry + entry->size + 4);

		
	}



    print("Kernel Version 0.0.1\n");

    interrupts_init();
    print("Interrupt system initialization: Success.\n");

    idt_init();
    print("IDT system initialization: Success.\n");

	paging_init();
	print("Paging system initialization: success\n");
	print("__________________________________________________\n");
	print("Testing custom virtual mapping...\n");
	volatile uint32_t *test_address = (uint32_t *)0x00400000;
	*test_address = 0x12345678;
	print("writing successful!\n");

	uint32_t value = *(volatile uint32_t *)test_frame_address;
	print("read through physical identity mapping\n");
	print_hex(value);
	print("\n");

	print("\nTesting automatic page table creation....\n");
	uint32_t new_frame = allocate_frame();
	
	print("new physical frame: ");
	print_hex(new_frame);
	print("\n");
	
	map_page(0x00800000, new_frame);
	print("mapping created succesfuly\n");
	
	volatile uint32_t *new_address = (uint32_t *)0x00800000;;
	*new_address = 0xCAFEBABE;

	
	print("testing filly dynamic mapping------\n");
	
	uint32_t dynamic_frame = allocate_frame();
	print("Dynamic frame: ");
	print_hex(dynamic_frame);
	print("\n");
	
	map_page(0x00800000, dynamic_frame);
	volatile uint32_t *dynamic_address = (uint32_t *)0x00800000;
	*dynamic_address = 0xCAFEBABE;
	print("Dynamic write succesful!\n");
	
	uint32_t dynamic_value = *(volatile uint32_t *)dynamic_frame;

	print("Read through identity mapping: ");
	print_hex(dynamic_value);
	print("\n");
	
	print("testing gpf/////////////\n");


















	//volatile uint32_t *ptr = (uint32_t *)0xDEADBEEF;
	//*ptr =123;

/*
	__asm__ volatile(
		"mov $0x20, %ax\n"
		"mov %ax, %ds\n"
	);*/
/*	print("testing devide by 0 exception/////////////////////////////////// \n");
	volatile int x = 10;
	volatile int y = 0;
	volatile int z = x/y;
	(void)z;	
*/
  /*  print("\n");
    print("Mood-Amber OS is alive!\n");
*/
    while (1)
    {
    }
}
