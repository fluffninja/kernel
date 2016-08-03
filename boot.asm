%define STAGE_0_1_SIGNATURE_VALUE   0xaa55
%define STAGE_0_1_SIZE              512
%define STAGE_0_SECTOR              0x07c0
%define STAGE_1_SECTOR              0x8000


    [bits   16]
    [org    0]


    jmp         stage_0_start_16

STR_STAGE_1_OK              db  "Entering Stage 1", 0
STR_STAGE_1_FLOPPY_READ     db  "Reading Floppy", 0
STR_STAGE_1_FLOPPY_OK       db  "Floppy OK", 0
STR_STAGE_1_FLOPPY_ERROR    db  "Floppy Error: ", 0
STR_STAGE_1_FLOPPY_FATAL    db  "Fatal Floppy Error", 0

DIGIT_VALUES                db  "0123456789ABCDEF"


stage_0_start_16:
    ; Check the bootloader signature prior to moving
    mov         ax, STAGE_0_SECTOR
    mov         ds, ax
    mov         ax, [STAGE_0_1_SIGNATURE]
    cmp         ax, STAGE_0_1_SIGNATURE_VALUE
    jne         short .fail

    ; Copy stages 0 and 1 to a new location
    mov         ax, STAGE_1_SECTOR
    mov         es, ax
    xor         si, si
    xor         di, di
    mov         cx, (STAGE_0_1_SIZE / 2)            ; Divide by 2 because words
    cld
    rep movsw    

    ; Check the bootloader signature after moving
    mov         ax, STAGE_1_SECTOR
    mov         ds, ax
    mov         ax, [STAGE_0_1_SIGNATURE]
    cmp         ax, STAGE_0_1_SIGNATURE_VALUE
    jne         short .fail

    jmp         long STAGE_1_SECTOR:stage_1_start_16

.fail:
    ; Print a solitary 'X' to denote our abject failure
    mov         bx, 0x0007
    mov         ah, 0x0e
    mov         al, 'X'
    int         0x10
    ; Wait for keyboard input and restart
    xor         ax, ax
    int         0x16
    int         0x19


stage_1_start_16:
    cli
    mov         ax, cs
    mov         ds, ax
    mov         es, ax
    mov         sp, ax
    xor         ax, ax
    mov         ss, ax
    sti    

    mov         si, STR_STAGE_1_OK
    call        print_line_16

    call        print_drive_status
 
    call        hang_16


; Print a hexadecimal byte value
; AL: Byte value
print_hex_16:
    pusha
    mov         ah, 0x0e
    mov         bx, 0x0007
    push        ax
    shr         al, 4
    movzx       si, al
    add         si, DIGIT_VALUES
    mov         al, [si]
    int         0x10
    pop         ax
    and         al, 0x0f
    movzx       si, al
    add         si, DIGIT_VALUES
    mov         al, [si]
    int         0x10
    popa
    ret


move_to_new_line_16:
    pusha
    mov         bh, 0
    mov         ah, 0x03
    int         0x10
    xor         dl, dl
    inc         dh
    mov         ah, 0x02
    int         0x10
    popa
    ret


; Print a null-terminated string
; SI: String Address
print_string_16:
    pusha
    mov         ax, 0x0e00
    mov         bx, 0x0007
.lop:
    lodsb
    cmp         al, 0
    je          short .done
    int         0x10
    jmp         short .lop
.done:
    popa
    ret


print_line_16:
    call        print_string_16
    call        move_to_new_line_16
    ret


; Print the current status of the drive
; DL: Drive
print_drive_status:
    pusha
    mov         ah, 0x01
    int         0x13
    jnc         short .no_error
    mov         si, STR_STAGE_1_FLOPPY_ERROR
    call        print_string_16
    mov         cl, ah
    call        print_hex_16
    jmp         short .done
.no_error:
    mov         si, STR_STAGE_1_FLOPPY_OK
    call        print_string_16
.done:
    call        move_to_new_line_16
    popa
    ret


hang_16:
    hlt
    jmp         $


times (512-2)-($-$$)                hlt
STAGE_0_1_SIGNATURE                 dw  STAGE_0_1_SIGNATURE_VALUE
