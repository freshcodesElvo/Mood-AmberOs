
section .multiboot
align 4

    dd 0x1BADB002
    dd 0x00
    dd -(0x1BADB002)

section .bss
align 16

stack_bottom:
    resb 16384
stack_top:

section .text

global start
extern kernel_main

start:
    cli
    cld

    ; Preserve Multiboot information
    mov esi, eax
    mov edi, ebx

    ; Setup kernel stack
    mov esp, stack_top

    ; Load our GDT
    lgdt [gdt_descriptor]

    ; Reload code segment
    jmp 0x08:flush_gdt

flush_gdt:

    ; Reload data segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Restore stack after loading SS
    mov esp, stack_top

    ; Pass arguments to kernel_main
    ; cdecl: push second argument, then first
    push edi
    push esi

    call kernel_main

    add esp, 8

hang:
    cli
    hlt
    jmp hang


section .rodata
align 8

; ==================================
; GLOBAL DESCRIPTOR TABLE
; ==================================

gdt_start:

    ; Null descriptor
    dq 0x0000000000000000

    ; Kernel code segment: selector 0x08
    dq 0x00CF9A000000FFFF

    ; Kernel data segment: selector 0x10
    dq 0x00CF92000000FFFF

gdt_end:

; ==================================
; GDT DESCRIPTOR
; ==================================

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

