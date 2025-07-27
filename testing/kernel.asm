[BITS 32]
[ORG 0x1000]

; Simple test kernel entry point
kernel_main:
    call clear_screen
    
    mov esi, kernel_msg
    mov edi, 0xB8000
    call print_string
    
    call draw_pattern
    
    jmp hang

clear_screen:
    push eax
    push ecx
    push edi
    
    mov edi, 0xB8000        
    mov eax, 0x0F200F20     
    mov ecx, 80 * 25 / 2    
    
    cld
    rep stosd               
    
    pop edi
    pop ecx
    pop eax
    ret

print_string:
    push eax
    push esi
    push edi
    
.loop:
    lodsb                   
    cmp al, 0
    je .done
    
    cmp al, 0x0A            
    je .newline
    
    mov ah, 0x0F            
    stosw                  
    jmp .loop
    
.newline:
    mov eax, edi
    sub eax, 0xB8000        
    mov ebx, 160            
    xor edx, edx
    div ebx                 
    inc eax                 
    mul ebx                 
    add eax, 0xB8000
    mov edi, eax
    jmp .loop
    
.done:
    pop edi
    pop esi
    pop eax
    ret

draw_pattern:
    push eax
    push ebx
    push ecx
    push edi
    
    mov edi, 0xB8000 + (160 * 3) 
    mov ecx, 16                    
    mov bl, 0x10                  
    
.pattern_loop:
    mov al, ' '             
    mov ah, bl              
    stosw                   
    stosw                  
    inc bl                  
    cmp bl, 0x80            
    jl .no_reset
    mov bl, 0x10            
.no_reset:
    loop .pattern_loop
    
    mov edi, 0xB8000 + (160 * 5)
    mov esi, ascii_art
    call print_string
    
    pop edi
    pop ecx
    pop ebx
    pop eax
    ret

hang:
    hlt
    jmp hang

kernel_msg db 'Kyro OS Test Kernel Loaded Successfully!', 0x0A
           db 'Protected mode is working!', 0x0A, 0x0A, 0

ascii_art  db '  ___  ___  ___  ___    ___  ___', 0x0A
           db ' |  _ |_  _|  _ |  _ |  / _ |  ___|', 0x0A  
           db ' | |_) || || |_)|  | | | | |___', 0x0A
           db ' |  _ ( || ||  _ |  | | |_| |___|', 0x0A
           db ' |_| |_||_||_| |_|_|  |___|_____|', 0x0A
           db 0

times 1024-($-$$) db 0
