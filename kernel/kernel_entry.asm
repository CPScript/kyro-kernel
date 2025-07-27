[BITS 32]
[EXTERN kernel_main]
[GLOBAL _start]

_start:
    ; Set up segment registers for kernel
    mov ax, 0x10    ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack
    mov esp, 0x90000
    mov ebp, esp
    
    ; Call C kernel main function
    call kernel_main
    
    ; Hang if kernel returns
    cli
    hlt
    jmp $
