#include "interrupt_handlers.h"
#include "screen.h"

void kernel_exception(
    const char *name,
    uint32_t vector,
    struct interrupt_frame *frame,
    uint32_t error_code
)
{
    print("\n\n");
    print("========================================\n");
    print("              KERNEL PANIC\n");
    print("========================================\n");

    print("Exception : ");
    print(name);
    print("\n");

    print("Vector    : ");
    print_hex(vector);
    print("\n");

    print("Error Code: ");
    print_hex(error_code);
    print("\n");

    print("EIP       : ");
    print_hex(frame->eip);
    print("\n");

    print("CS        : ");
    print_hex(frame->cs);
    print("\n");

    print("EFLAGS    : ");
    print_hex(frame->eflags);
    print("\n");

    print("\nCPU HALTED\n");

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}
