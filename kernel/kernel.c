#include  <stdint.h>
#include "screen.h"
#include "interrupts.h"
#include "idt.h"
#include "paging.h"
#include "multiboot.h"
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

	struct multiboot_mmap_entry *entry = 
		(struct multiboot_mmap_entry *)
		(*(uint32_t *)(multiboot_info_addr + 48));

	uint32_t mmap_end = 
		(*(uint32_t *)(multiboot_info_addr + 48))+
		(*(uint32_t *)multiboot_info_addr + 44);

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


	print("testing gpf/////////////\n");

	volatile uint32_t *ptr = (uint32_t *)0xDEADBEEF;
	*ptr =123;

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
