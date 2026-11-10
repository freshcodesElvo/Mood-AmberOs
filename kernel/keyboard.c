#include <stdint.h>
#include "keyboard.h"
#include "screen.h"

//read a byte fro m an i/o port
static inline uint8_t inb(uint16_t port){
	uint8_t result;
	__asm__ volatile(
		"inb %1, %0"	
		: "=a"(result)
		: "Nd"(port)
	);
	return result;
}

//write a byte into an i/o port

static inline void outb(uint16_t port, uint8_t value){
	__asm__ volatile (
		"outb %0,%1"
		:
		: "a"(value), "Nd"(port)
	);
}

void keyboard_handler(void){
	//read the keybord scancode
	uint8_t scancode = inb(0x60);
	print("keyboard scancode: ");
	print_hex(scancode);
	print("\n");

	//tell the master pci taht the inerrupt was handled
	outb(0x20, 0x20);
}
