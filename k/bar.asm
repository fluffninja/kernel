    [bits 32]

    [org 0x7c00 + 512 * 2]

    mov         ecx, 80 * 25
    mov         ax, '!' | (0x04 << 8)
repeat:
    mov         word [0xb8000 + (ecx - 1) * 2], ax
    loop        repeat

    cli
    hlt
