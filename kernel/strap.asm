;
; strap.asm - Kernel Bootstrap
;
; Calls kmain

    [bits       32]
    [section    .text]

    [extern     kmain]

    push        kmain
    ret
