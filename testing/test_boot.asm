[BITS 16]
[ORG 0x7C00]
start:
    mov ax, 0x0000
    mov ds, ax
    mov si, msg
    mov ah, 0x0E
loop: lodsb
    cmp al, 0
    je hang
    int 0x10
    jmp loop
hang: hlt
    jmp hang
msg db "TEST BOOTLOADER OK", 0
times 510-($-$) db 0
dw 0xAA55
