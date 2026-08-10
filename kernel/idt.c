#include "idt.h"

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;


void idt_init(void){
	idtp.limit = sizeof(idt)-1;
	idtp.base = (uint32_t)&idt;
	
	for(int i =0;i<IDT_ENTRIES;i++){
		idt[i].offset_low = 0;
		idt[i].selector = 0;
		idt[i].zero = 0;
		idt[i].type_attr = 0;
		idt[i].offset_high = 0;
	}
	idt_load((uint32_t)&idtp);
}
