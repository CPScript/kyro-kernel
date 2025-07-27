[BITS 16]
[ORG 0x7C00]

KERNEL_OFFSET equ 0x1000
KERNEL_SECTORS equ 50

start:
    ; Initialize segment registers
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Clear screen
    mov ah, 0x00
    mov al, 0x03
    int 0x10

    ; Print boot message
    mov si, boot_msg
    call print_string

    ; Reset disk drive
    mov ah, 0x00
    mov dl, 0x80
    int 0x13
    jc disk_error

    ; Load kernel using LBA
    call load_kernel

setup_protected_mode:
    ; Set up basic GDT for protected mode
    cli
    lgdt [gdt_descriptor]

    ; Enable A20 line
    call enable_a20

    ; Switch to protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Far jump to flush CPU pipeline and enter protected mode
    jmp CODE_SEG:init_pm

disk_error:
    mov si, disk_error_msg
    call print_string
    jmp hang

; Load kernel using LBA addressing
load_kernel:
    mov si, loading_msg
    call print_string

    ; Set up disk address packet
    mov si, disk_address_packet
    mov ah, 0x42
    mov dl, 0x80
    int 0x13
    jc disk_error

    mov si, kernel_loaded_msg
    call print_string
    ret

; Disk Address Packet (DAP)
disk_address_packet:
    db 0x10            ; Size of packet
    db 0x00            ; Reserved
    dw KERNEL_SECTORS  ; Number of sectors to read
    dw KERNEL_OFFSET   ; Offset (destination address)
    dw 0x0000          ; Segment
    dq 1               ; Starting LBA (sector 1)

; Print string (real mode)
print_string:
    mov ah, 0x0E
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    ret

; Enable A20 line
enable_a20:
    in al, 0x92
    test al, 2
    jnz .done
    or al, 2
    and al, 0xFE
    out 0x92, al
.done:
    ret

; 32-bit protected mode entry
[BITS 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    mov esi, pm_msg
    call print_string_pm

    call KERNEL_OFFSET

    jmp hang

; Print string (protected mode)
print_string_pm:
    mov edx, 0xb8000
.loop:
    mov al, [esi]
    cmp al, 0
    je .done
    mov ah, 0x0F
    mov [edx], ax
    add edx, 2
    inc esi
    jmp .loop
.done:
    ret

hang:
    jmp hang

; GDT
gdt_start:
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Strings
boot_msg db 'Kyro OS - Bootloader Starting...', 0x0D, 0x0A, 0
loading_msg db 'Loading kernel...', 0x0D, 0x0A, 0
kernel_loaded_msg db 'Kernel loaded successfully!', 0x0D, 0x0A, 0
disk_error_msg db 'Disk read error!', 0x0D, 0x0A, 0
pm_msg db 'Entered 32-bit protected mode', 0

; Boot sector signature
times 510-($-$$) db 0
dw 0xAA55
