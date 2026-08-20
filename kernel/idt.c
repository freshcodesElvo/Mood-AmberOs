#include "idt.h"
#include "interrupt_handlers.h"

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;

static void idt_set_gate(
    int number,
    uint32_t handler,
    uint16_t selector,
    uint8_t type_attr
)
{
    idt[number].offset_low = handler & 0xFFFF;
    idt[number].selector = selector;
    idt[number].zero = 0;
    idt[number].type_attr = type_attr;
    idt[number].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_init(void)
{
	asm volatile("cli");
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;

    for (int i = 0; i < IDT_ENTRIES; i++)
    {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
        idt[i].offset_high = 0;
    }

    idt_set_gate(0, (uint32_t)divide_by_zero_handler, 0x10, 0x8E);
	idt_set_gate(13, (uint32_t)gpf_handler, 0x10, 0x8E);
    	

    idt_load((uint32_t)&idtp);
}
