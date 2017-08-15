[bits 32]

global get_cr3
global set_cr3
global get_cr0
global set_cr0
global enter_high_half
global get_ip

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

get_cr3:
    mov         eax, cr3
    ret
    
set_cr3:
    mov         eax, [esp+4]
    mov         cr3, eax
    ret

get_cr0:
    mov         eax, cr0
    ret

set_cr0:
    mov         eax, [esp+4]
    mov         cr0, eax
    ret

enter_high_half:
    pop         eax
    add         eax, 0x80000000
    push        eax
    ret

get_ip:
    pop         eax
    push        eax
    ret
