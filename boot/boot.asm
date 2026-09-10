		
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
	mov esp, stack_top
	; Save Multiboot values

	mov ecx, eax
	mov edx, ebx

	push edx
	push ecx
	call kernel_main

	add esp, 8

hang:
	cli
	hlt
	jmp hang
