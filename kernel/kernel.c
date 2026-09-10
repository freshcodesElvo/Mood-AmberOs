	#include  <stdint.h>
#include "screen.h"
#include "interrupts.h"
#include "idt.h"
#include "paging.h"
void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_info_addr)
{
    clear_screen();

    print("---------------------------------\n");
    print("MOOD-AMBER OS\n");
    print("---------------------------------\n\n");
	
	print("Multiboot magic >>>> ");
	print_hex(multiboot_magic);
	print("\n");
    print("multiboot information address: ");
    print_hex(multiboot_info_addr);
    print("\n");

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
