#include "screen.h"
#include "interrupts.h"
#include "idt.h"
#include "paging.h"
void kernel_main(void)
{
    clear_screen();

    print("---------------------------------\n");
    print("MOOD-AMBER OS\n");
    print("---------------------------------\n\n");

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

	print("You are not supposed to appear bruv!!!😂😂😂\n");
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
