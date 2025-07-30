[BITS 16]
[ORG 0x7C00]

KERNEL_OFFSET equ 0x1000
KERNEL_SECTORS equ 64

start:
    ; Setup segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7000

    ; Clear screen and show boot message
    mov ax, 0x0003
    int 0x10
    
    mov si, boot_msg
    call print_string

    ; Reset disk
    xor ah, ah
    xor dl, dl          ; Floppy drive
    int 0x13

load_kernel:
    mov si, loading_msg
    call print_string
    
    ; Load kernel in chunks to avoid BIOS limitations
    mov cx, KERNEL_SECTORS    ; Total sectors to read
    mov bx, KERNEL_OFFSET     ; Buffer address
    mov dl, 0x00              ; Drive (floppy)
    mov dh, 0                 ; Head 0
    mov ch, 0                 ; Cylinder 0  
    mov cl, 2                 ; Start at sector 2

load_loop:
    push cx                   ; Save remaining sectors
    
    ; Read maximum 18 sectors at a time (floppy track limit)
    mov ax, cx
    cmp ax, 18
    jle read_sectors
    mov ax, 18

read_sectors:
    push ax                   ; Save sectors to read
    mov ah, 0x02              ; Read sectors function
    ; AL already contains number of sectors
    int 0x13
    jc disk_error
    
    pop ax                    ; Restore sectors read
    pop cx                    ; Restore remaining sectors
    
    ; Update for next iteration
    sub cx, ax                ; Subtract sectors read
    jz kernel_loaded          ; Done if zero
    
    ; Update buffer pointer (sectors * 512 bytes)
    push dx
    mov dx, ax
    shl dx, 9                 ; Multiply by 512
    add bx, dx                ; Update buffer pointer
    pop dx
    
    ; Update sector number
    add cl, al                ; Next sector
    cmp cl, 18                ; End of track?
    jle load_loop
    
    ; Move to next track
    mov cl, 1                 ; Reset to sector 1
    inc dh                    ; Next head
    cmp dh, 2                 ; Floppy has 2 heads
    jl load_loop
    
    ; Move to next cylinder
    mov dh, 0                 ; Reset head
    inc ch                    ; Next cylinder
    jmp load_loop

disk_error:
    pop ax                    ; Clean up stack
    pop cx
    mov si, error_msg
    call print_string
    jmp hang

kernel_loaded:
    mov si, success_msg
    call print_string
    jmp setup_pm

setup_pm:
    cli
    lgdt [gdt_desc]
    
    ; Enable A20 (fast method)
    in al, 0x92
    or al, 2
    out 0x92, al
    
    ; Enter protected mode
    mov eax, cr0
    or al, 1
    mov cr0, eax
    
    jmp 0x08:pm_start

; Print string function
print_string:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

hang:
    hlt
    jmp hang

; Protected mode code
[BITS 32]
pm_start:
    ; Setup segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Setup stack
    mov esp, 0x90000
    
    ; Jump to kernel
    call KERNEL_OFFSET
    
    ; Should never reach here
    jmp $

; GDT
gdt_start:
    ; Null descriptor
    dq 0x0000000000000000
    
    ; Code descriptor: base=0, limit=4GB, present, ring 0, code, readable
    dq 0x00CF9A000000FFFF
    
    ; Data descriptor: base=0, limit=4GB, present, ring 0, data, writable
    dq 0x00CF92000000FFFF
gdt_end:

gdt_desc:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Messages
boot_msg db 'Kyro OS Loading...', 13, 10, 0
loading_msg db 'Reading kernel...', 13, 10, 0
success_msg db 'Kernel loaded!', 13, 10, 0
error_msg db 'Disk error!', 13, 10, 0

; Pad to 510 bytes and add boot signature
times 510-($-$$) db 0
dw 0xAA55
