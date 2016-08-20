
    ; The first two stages of the bootloader are 16-bit.
    [bits 16]

    ; Is added to the segment-offset address of all symbols.
    [org 0x7c00]


    ; The BIOS can place us at 07c0:0000 or 0000:7c00,
    ; depending on whether we booted from HDD or floppy.
    ; The following jump normalises things to the latter.
    jmp         long 0x0000:stage0_start_16


    ; Stringsies
MSG_NO_FLOPPY_EXTENSIONS    db "No disk extensions", 0
MSG_PRESS_ANY_KEY_TO_REBOOT db "Any key reboots", 0
MSG_FLOPPY_READ_FAIL        db "Floppy error: ", 0


    ; Given to BIOS disk functions to reference the boot device
g_boot_device_b             db 0


stage0_start_16:
    ; Set up segment registers and the stack.
    ; Don't allow interrupts whilst doing this.
    cli
    xor         ax, ax
    mov         es, ax
    mov         ds, ax
    mov         ss, ax
    xor         sp, sp
    sti

    ; The BIOS leaves the ID of the boot device in DL.
    ; Save it for future use with BIOS functions.
    mov         byte [g_boot_device_b], dl

    ; Try to detect whethere this BIOS supports the
    ; floppy extension functions.
    mov         ah, 0x41
    mov         bx, 0x55aa
    xor         cx, cx
    stc 
    int         0x13
    jc          short .no_floppy_extensions
    cmp         bx, 0xaa55
    jne         short .no_floppy_extensions
    test        cx, 1
    jz          short .no_floppy_extensions

    ; Use the extended BIOS functions to load sectors
    ; off of the disk.
    ; Details of what to load are found in the disk
    ; address packed (DAP) - defined below.
    mov         ah, 0x42
    mov         si, .disk_address_packet_data
    int         0x13
    jc          .read_fail

    jmp         long 0x0000:stage1_start_16

.disk_address_packet_data:
    db          0x10                ; Size of packet
    db          0                   ; Reserved
    dw          3                   ; Sectors to read
    dw          (0x7c00 + 512)      ; Destination segment offet
    dw          0                   ; Destination segment
    dq          1                   ; On-disk sector index

.no_floppy_extensions:
    mov         si, MSG_NO_FLOPPY_EXTENSIONS
    call        print_line_16
    jmp         short .reboot

.read_fail:
    mov         si, MSG_FLOPPY_READ_FAIL
    call        print_string_16
    mov         bl, ah
    call        print_hex_byte_16
    call        print_new_line_16

.reboot:
    mov         si, MSG_PRESS_ANY_KEY_TO_REBOOT
    call        print_line_16

    xor         ah, ah
    int         0x16    ; Wait for a key-press
    int         0x19    ; Warm reboot

    cli
    hlt                 ; If the interrupts fail, halt


; Print a null-terminated string to the screen
; IN:
;   SI: String segment offset address
print_string_16:
    pusha
    mov         ah, 0x0e
.repeat:
    lodsb
    test        al, al
    jz          short .done
    int         0x10
    jmp         short .repeat
.done:
    popa
    ret


; Moves the text cursor to the beginning of the next line
print_new_line_16:
    pusha
    mov         ah, 0x0e
    xor         bh, bh
    mov         al, 10
    int         0x10
    mov         al, 13
    int         0x10
    popa
    ret


; Print a null-terminated string to the screen and move the 
; cursor to the next line
; IN:
;   SI: String segment offset address
print_line_16:
    call        print_string_16
    call        print_new_line_16
    ret


; Print a hexadecimal value to the screen
; IN:
;   BL: Value to convert and print
print_hex_byte_16:
    pusha
    mov         ah, 0x0e
    xor         bh, bh
    mov         al, bl
    shr         al, 4
    movzx       si, al
    mov         al, [si + .HEX_DIGITS]
    int         0x10
    mov         al, bl
    and         al, 0x0f
    movzx       si, al
    mov         al, [si + .HEX_DIGITS]
    int         0x10
    popa
    ret

.HEX_DIGITS                     db "0123456789ABCDEF"


times ((512 - 2) - ($ - $$))    db 0
                                dw 0xaa55


stage1_start_16:
    cli
    lgdt        [gdt_48]
    lidt        [idt_48]

    mov         eax, cr0
    or          eax, 0x1
    mov         cr0, eax

    jmp         0x08:stage2_start_32


; Define a GDT entry
; Params: Base-32, Limit-20, Access-8, Flags-4
%macro m_gdt_entry 4
    dw ((%2) & 0xffff)                                ; Limit   0:16
    dw ((%1) & 0xffff)                                ; Base    0:16
    db (((%1) >> 16) & 0xff)                          ; Base   16:24
    db ((%3) & 0xff)                                  ; Access  0:8
    db ((((%4) & 0x0f) << 4) | (((%2) >> 16) & 0x0f)) ; Limit  16:20, Flags 0:4
    db ((%1) >> 24)                                   ; Base   24:32
%endmacro


gdt:
.start:
    m_gdt_entry 0, 0, 0, 0                      ; Dummy Entry (must be present)
    m_gdt_entry 0, 0xfffff, 0b10011010, 0b1100  ; Text Segment
    m_gdt_entry 0, 0xfffff, 0b10010010, 0b1100  ; Data Segment
.end:


gdt_48:
    dw (gdt.end - gdt.start - 1)
    dd gdt


idt_48:
    dw 0
    dd 0


    [bits 32]


stage2_start_32:
    mov         ax, 0x10
    mov         ds, ax
    mov         ss, ax
    mov         es, ax
    mov         esp, 0x90000

    push        0x7c00 + 512 * 2
    ret


times ((512 * 2) - ($ - $$))    db 0
