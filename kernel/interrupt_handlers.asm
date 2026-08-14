global divide_by_zero_handler
extern divide_by_zero_handler_c

section .text

divide_by_zero_handler:
	cli
	call divide_by_zero_handler_c
hang_forever
	hlt
	jmp hang_forever

