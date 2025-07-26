[BITS 32]

; Import C functions
extern fault_handler
extern timer_handler
extern keyboard_interrupt_handler

; Export assembly handlers
global divide_error_handler_asm
global debug_handler_asm
global nmi_handler_asm
global breakpoint_handler_asm
global general_protection_fault_handler_asm
global page_fault_handler_asm
global timer_handler_asm
global keyboard_handler_asm

; Exception handlers without error codes
%macro ISR_NOERRCODE 2
global %1
%1:
    cli
    push byte 0        ; Push dummy error code
    push byte %2       ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Exception handlers with error codes
%macro ISR_ERRCODE 2
global %1
%1:
    cli
    push byte %2       ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Hardware interrupt handlers
%macro IRQ 2
global %1
%1:
    cli
    push byte 0        ; Push dummy error code
    push byte %2       ; Push IRQ number
    jmp irq_common_stub
%endmacro

; Define exception handlers
ISR_NOERRCODE divide_error_handler_asm, 0
ISR_NOERRCODE debug_handler_asm, 1
ISR_NOERRCODE nmi_handler_asm, 2
ISR_NOERRCODE breakpoint_handler_asm, 3
ISR_ERRCODE general_protection_fault_handler_asm, 13
ISR_ERRCODE page_fault_handler_asm, 14

; Define hardware interrupt handlers
IRQ timer_handler_asm, 32
IRQ keyboard_handler_asm, 33

; Common exception handler stub
isr_common_stub:
    pusha              ; Push all general purpose registers
    
    mov ax, ds         ; Save data segment
    push eax
    
    mov ax, 0x10       ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    call fault_handler ; Call C fault handler
    
    pop eax            ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa               ; Restore registers
    add esp, 8         ; Clean up error code and interrupt number
    sti
    iret               ; Return from interrupt

; Common IRQ handler stub
irq_common_stub:
    pusha              ; Push all general purpose registers
    
    mov ax, ds         ; Save data segment
    push eax
    
    mov ax, 0x10       ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Get interrupt number from stack
    mov eax, [esp + 36] ; Get interrupt number (after pusha + segment)
    
    cmp eax, 32        ; Timer interrupt?
    je .timer
    cmp eax, 33        ; Keyboard interrupt?
    je .keyboard
    jmp .end
    
.timer:
    call timer_handler
    jmp .end
    
.keyboard:
    call keyboard_interrupt_handler
    jmp .end
    
.end:
    ; Send EOI to PIC
    mov al, 0x20
    out 0x20, al       ; Send EOI to master PIC
    
    pop eax            ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa               ; Restore registers
    add esp, 8         ; Clean up error code and interrupt number
    sti
    iret               ; Return from interrupt
