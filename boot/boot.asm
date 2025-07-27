[BITS 16]
[ORG 0x7C00]

KERNEL_OFFSET equ 0x1000
KERNEL_SECTORS equ 50

start:
    ; Initialize segments
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7000

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

    ; Jump to setup protected mode
    jmp setup_protected_mode

disk_error:
    mov si, disk_error_msg
    call print_string
    jmp hang

; Load kernel using LBA addressing
load_kernel:
    mov si, loading_msg
    call print_string

    ; First check if LBA is supported
    mov ah, 0x41         ; Check extensions present
    mov bx, 0x55AA       ; Magic number
    mov dl, 0x80         ; Drive number
    int 0x13
    jc .try_chs          ; If carry set, LBA not supported
    
    ; LBA is supported, proceed with LBA read
    mov si, disk_address_packet
    mov ah, 0x42         ; Extended read with LBA
    mov dl, 0x80         ; Disk number (first hard disk)
    int 0x13
    jc disk_error        ; Jump to disk_error if disk read fails
    jmp .success

.try_chs:
    ; Fallback to CHS (Cylinder-Head-Sector) addressing
    mov si, chs_msg
    call print_string
    
    ; Read using CHS: Cylinder 0, Head 0, Sector 2 (sectors start at 1)
    mov ah, 0x02         ; Read sectors function
    mov al, KERNEL_SECTORS ; Number of sectors to read (limited to 63 for CHS)
    mov ch, 0            ; Cylinder 0
    mov cl, 2            ; Sector 2 (kernel starts after boot sector)
    mov dh, 0            ; Head 0
    mov dl, 0x80         ; Drive number
    mov bx, KERNEL_OFFSET ; Where to load the kernel
    int 0x13
    jc disk_error

.success:
    ; Success message after kernel is loaded
    mov si, kernel_loaded_msg
    call print_string
    ret

; Disk Address Packet (DAP) for LBA
disk_address_packet:
    db 0x10              ; Size of packet (16 bytes)
    db 0x00              ; Reserved
    dw KERNEL_SECTORS    ; Number of sectors to read
    dw KERNEL_OFFSET     ; Offset (destination address for kernel)
    dw 0x0000            ; Segment (where to load - segment 0)
    dq 1                 ; Starting LBA (sector 1 - right after boot sector)

setup_protected_mode:
    ; Disable interrupts
    cli
    
    ; Load GDT
    lgdt [gdt_descriptor]

    ; Enable A20 line
    call enable_a20

    ; Switch to protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Far jump to flush CPU pipeline and enter protected mode
    jmp CODE_SEG:init_pm

; Print string (real mode)
print_string:
    push ax
    push si
    mov ah, 0x0E
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    pop si
    pop ax
    ret

; Enable A20 line using fast A20 gate
enable_a20:
    push ax
    
    ; Try fast A20 first
    in al, 0x92
    test al, 2
    jnz .done
    or al, 2
    and al, 0xFE
    out 0x92, al
    
.done:
    pop ax
    ret

; 32-bit protected mode code
[BITS 32]
init_pm:
    ; Set up segment registers for protected mode
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set up stack
    mov ebp, 0x90000
    mov esp, ebp

    ; Print protected mode message
    mov esi, pm_msg
    call print_string_pm

    ; Jump to kernel (make sure kernel exists at this address!)
    call KERNEL_OFFSET

    ; If kernel returns, hang
    jmp hang

; Print string in protected mode (VGA text mode)
print_string_pm:
    push eax
    push edx
    push esi
    
    mov edx, 0xB8000     ; VGA text buffer
.loop:
    mov al, [esi]
    cmp al, 0
    je .done
    mov ah, 0x0F         ; White on black
    mov [edx], ax
    add edx, 2
    inc esi
    jmp .loop
.done:
    pop esi
    pop edx
    pop eax
    ret

hang:
    hlt
    jmp hang

; Global Descriptor Table (GDT)
gdt_start:
    ; Null descriptor
    dd 0x0
    dd 0x0

gdt_code:
    ; Code segment descriptor
    dw 0xFFFF        ; Limit (bits 0-15)
    dw 0x0000        ; Base (bits 0-15)
    db 0x00          ; Base (bits 16-23)
    db 10011010b     ; Access byte: present, ring 0, code, executable, readable
    db 11001111b     ; Flags and limit (bits 16-19): 4KB granularity, 32-bit
    db 0x00          ; Base (bits 24-31)

gdt_data:
    ; Data segment descriptor
    dw 0xFFFF        ; Limit (bits 0-15)
    dw 0x0000        ; Base (bits 0-15)
    db 0x00          ; Base (bits 16-23)
    db 10010010b     ; Access byte: present, ring 0, data, writable
    db 11001111b     ; Flags and limit (bits 16-19): 4KB granularity, 32-bit
    db 0x00          ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1    ; Size of GDT
    dd gdt_start                  ; Address of GDT

; Segment selectors
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; String messages
boot_msg db 'Kyro OS - Bootloader Starting...', 0x0D, 0x0A, 0
loading_msg db 'Loading kernel...', 0x0D, 0x0A, 0
chs_msg db 'Using CHS addressing...', 0x0D, 0x0A, 0
kernel_loaded_msg db 'Kernel loaded successfully!', 0x0D, 0x0A, 0
disk_error_msg db 'Disk read error!', 0x0D, 0x0A, 0
pm_msg db 'Entered 32-bit protected mode', 0

; Pad to 510 bytes and add boot signature
times 510-($-$$) db 0
dw 0xAA55
