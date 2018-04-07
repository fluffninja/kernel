[bits 32]

global establish_pit
global cpu_hlt

cpu_hlt:
    hlt
    ret

establish_pit:

    pushad
    pushfd

    cli

    mov al, 00110100b
    out 0x43, al
    mov eax, [esp+4]
    out 0x40, al
    mov al, ah
    out 0x40, al
    
    sti

    popfd
    popad
    ret

