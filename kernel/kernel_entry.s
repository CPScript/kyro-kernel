[BITS 32]
GLOBAL _start
section .text.start
EXTERN kernel_main

_start:
    ; CRITICAL: First thing - set up a stable environment
    ; Set up segment registers (bootloader already set up GDT)
    mov ax, 0x10    ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack in safe high memory area
    mov esp, 0x90000   ; Use 576KB instead of 2MB
    mov ebp, esp
    
    ; Clear direction flag
    cld
    
    ; Clear interrupts until we're ready
    cli
    
    ; Print early boot message to verify we got here
    mov esi, boot_success_msg
    call print_early
    
    ; Call C kernel main function
    call kernel_main
    
    ; Hang if kernel returns
    cli
    hlt
    jmp $

; Early print function (before printf is initialized)
print_early:
    push eax
    push edx
    mov edx, 0xB8000
    mov ah, 0x0F
.loop:
    lodsb
    test al, al
    jz .done
    mov [edx], ax
    add edx, 2
    jmp .loop
.done:
    pop edx
    pop eax
    ret

boot_success_msg db 'Kernel entry point reached successfully', 13, 10, 0

; Add .note.GNU-stack section to suppress linker warning
section .note.GNU-stack noalloc noexec nowrite progbits