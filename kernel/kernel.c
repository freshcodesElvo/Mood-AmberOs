#include "screen.h"
#include "interrupts.h"
void kernel_main(void){

	clear_screen();

	print("---------------------------------\n");
	print("MOOD-AMBER OS\n");
	print("---------------------------------\n\n");

	print("Kernel Version 0.0.1\n");
	interrupts_init();

	print("interrupt syst initialization: Success. \n");
	print("\n");
	print("Mood-Amber OS is alive\n");

	while(1){
		
	}

}
