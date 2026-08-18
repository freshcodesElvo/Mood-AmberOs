#include "interrupt_handlers.h"
#include "screen.h"

void divide_by_zero_handler_c(struct interrupt_frame *frame){
	(void)frame;
  	print("\n\n");
	print("---------------------\n");
	print("kernel exception occured!");
	print("----------------------\n");
	print("divide by zero exeption \n");
	print("CPU stopped");

	while(1){__asm__ volatile("hlt");}
}
