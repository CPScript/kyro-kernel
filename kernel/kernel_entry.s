[BITS 32]

; Entry point for kernel
GLOBAL _start
EXTERN kernel_main

section .text.start
_start:
    ; Set up segment registers
    mov ax, 0x10    ; Data segment selector from bootloader's GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack
    mov esp, 0x90000
    mov ebp, esp
    
    ; Clear BSS section if needed
    ; Note: BSS clearing is handled by linker in this simple setup
    
    ; Call C kernel main function
    call kernel_main
    
    ; Hang if kernel returns
    cli
    hlt
    jmp $

; Add .note.GNU-stack section to suppress linker warning
section .note.GNU-stack noalloc noexec nowrite progbits