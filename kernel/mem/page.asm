[bits 32]

global initpaging
global get_cr3
global set_cr3

;Only have 4 entries for kernel page directory right now, so only pointers & 0x00c03fff are valid
kernel_page_directory:
    dd          0
    dd          1
    dd          2
    dd          3

kernel_page_table:
    dd          0
    dd          1
    dd          2
    dd          3

initpaging:
    push        ebp
    mov         ebp, esp
    ;Stack:
    ;   $
    sub         esp, 0x0



    mov         esp, ebp
    pop         ebp
    ret

get_cr3:
    mov         eax, cr3
    ret
    
set_cr3:
    mov         eax, [ebp+8]
    mov         cr3, eax
    ret