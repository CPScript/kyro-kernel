[BITS 32]

; Simple test kernel to verify bootloader works
section .text
global _start

_start:
    ; Set up segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack
    mov esp, 0x90000
    
    ; Clear screen with colored background to show we're in kernel
    mov edi, 0xB8000
    mov ecx, 80 * 25
    mov ax, 0x4F20      ; White on red background, space character
    rep stosw
    
    ; Display success message
    mov esi, kernel_msg
    mov edi, 0xB8000
    mov ah, 0x0F        ; White on black
    
print_loop:
    lodsb
    test al, al
    jz done
    stosw
    jmp print_loop
    
done:
    ; Infinite loop to prevent reboot
    jmp done

kernel_msg db 'loaded', 0

; Pad to make sure we have some content
times 1024-($-$$) db 0