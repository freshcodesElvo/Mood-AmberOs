global divide_by_zero_handler
global gpf_handler

extern kernel_exception

section .text

; ==========================================
; Divide by Zero - Exception 0
; ==========================================

divide_by_zero_handler:
    cli

    ; CPU pushed:
    ; [ESP]     = EIP
    ; [ESP + 4] = CS
    ; [ESP + 8] = EFLAGS

    push dword 0
    push esp

    ; The stack now contains:
    ; [ESP]     = pointer to frame
    ; [ESP + 4] = error code

    push dword 0
    push esp

    ; This arrangement is getting unnecessarily complicated,
    ; so for now we will use a dedicated C-compatible layout.

    add esp, 8

divide_hang:
    hlt
    jmp divide_hang


; ==========================================
; General Protection Fault - Exception 13
; ==========================================

gpf_handler:
    cli

    ; CPU pushed:
    ; [ESP]     = error code
    ; [ESP + 4] = EIP
    ; [ESP + 8] = CS
    ; [ESP + 12] = EFLAGS

    mov eax, esp
    add eax, 4

    ; eax now points to:
    ; EIP
    ; CS
    ; EFLAGS

    push dword [esp]
    push eax
    push dword 13
    push gpf_name

    call kernel_exception

    add esp, 16

gpf_hang:
    hlt
    jmp gpf_hang


section .rodata

divide_by_zero_name:
    db "Divide by zero", 0

gpf_name:
    db "General Protection Fault", 0
