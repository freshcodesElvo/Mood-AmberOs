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
static const char scancode_table[128]={
	 [0x02] = '1',
   	 [0x03] = '2',
   	 [0x04] = '3',
   	 [0x05] = '4',
   	 [0x06] = '5',
   	 [0x07] = '6',
   	 [0x08] = '7',
   	 [0x09] = '8',
   	 [0x0A] = '9',
   	 [0x0B] = '0',

   	 [0x10] = 'q',
  	 [0x11] = 'w',
   	 [0x12] = 'e',
   	 [0x13] = 'r',
   	 [0x14] = 't',
   	 [0x15] = 'y',
  	 [0x16] = 'u',
   	 [0x17] = 'i',
   	 [0x18] = 'o',
	 [0x19] = 'p',

 	 [0x1E] = 'a',
    	 [0x1F] = 's',
    	 [0x20] = 'd',
       	 [0x21] = 'f',
   	 [0x22] = 'g',
  	 [0x23] = 'h',
    	 [0x24] = 'j',
    	 [0x25] = 'k',
   	 [0x26] = 'l',

   	 [0x2C] = 'z',
   	 [0x2D] = 'x',
   	 [0x2E] = 'c',
   	 [0x2F] = 'v',
   	 [0x30] = 'b',
   	 [0x31] = 'n',
   	 [0x32] = 'm',

   	 [0x39] = ' '
};

void keyboard_handler(void){
	//read the keybord scancode
	uint8_t scancode = inb(0x60);
	//ignore key-release events
	if(scancode & 0x80){
		outb(0x20,0x20);
		return;
	}
	//trnslate scancode into character
	char character = scancode_table[scancode];
	if(character !=0){
		print("key pressed: ");
		print_char(character);
		print("\n");
	}
	//send end of interrupt to the master PIC
	//tell the master pci taht the inerrupt was handled
	outb(0x20, 0x20);
}
