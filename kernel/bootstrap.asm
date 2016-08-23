;
; bootstrap.asm - Kernel Bootstrap
;
; Calls kmain

    [bits       32]
    [section    .text]

    [extern     kmain]

    jmp         kmain
