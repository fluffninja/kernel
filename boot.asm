%define STAGE_0_1_SIGNATURE_VALUE   0xaa55
%define STAGE_2_SIGNATURE_VALUE     0x1337
%define STAGE_0_1_SIZE              512
%define STAGE_0_SECTOR              0x07c0
%define STAGE_1_SECTOR              0x8000
%define STAGE_2_SECTOR              0x1000
%define STAGE_1_FLOPPY_ATTEMPTS     5


    [bits   16]
    [org    0]


    jmp         STAGE_0_SECTOR:stage_0_start_16


STR_STAGE_1_OK              db  "Entering Stage 1", 13, 10, 0
STR_STAGE_1_FLOPPY_READ     db  "Reading from Floppy", 13, 10, 0
STR_STAGE_1_FLOPPY_ERROR    db  "Floppy Error", 13, 10, 0
STR_STAGE_1_FLOPPY_FATAL    db  "Fatal Floppy Error", 13, 10, 0
STR_STAGE_2_OK              db  "Entering Stage 2", 13, 10, 0

b_boot_drive_id             db  0x00


stage_0_start_16:
    mov         [b_boot_drive_id], dl

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
    call        print_string_16

    mov         ax, STAGE_2_SECTOR
    mov         es, ax
    mov         al, 1
    mov         ah, 0x02
    mov         bx, 0
    mov         cl, 2
    mov         ch, 0
    mov         dl, [b_boot_drive_id]
    mov         dh, 1
    int         0x13
    jc          short .fail

    jmp         long STAGE_2_SECTOR:0

.fail:
    mov         si, STR_STAGE_1_FLOPPY_FATAL
    call        print_string_16
    xor         ax, ax
    int         0x16
    int         0x19



stage_1_read_floppy_16:
    pusha
    mov         si, (STAGE_1_FLOPPY_ATTEMPTS + 1)

.lop:
    dec         si
    jz          short .fail

    push        si
    mov         si, STR_STAGE_1_FLOPPY_READ
    call        print_string_16
    pop         si

    mov         ax, STAGE_2_SECTOR
    mov         es, ax

    mov         al, 1
    mov         ah, 0x02
    mov         bx, 0
    mov         cl, 2
    mov         ch, 1
    mov         dl, [b_boot_drive_id]
    mov         dh, 0

    int         0x13
    jnc         short .success

    push        si
    mov         si, STR_STAGE_1_FLOPPY_ERROR
    call        print_string_16
    pop         si

    xor         ah, ah
    int         0x13
    jnc         short .lop

.success:    
    clc
    jmp         short .done

.fail:
    stc

.done:
    popa
    ret


print_hex_16:
    pusha
    popa
    ret


print_string_16:
    pusha
    xor         ax, 0x0e00
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


hang_16:
    hlt
    jmp     hang_16


times (512-2)-($-$$)        db 0

STAGE_0_1_SIGNATURE         dw STAGE_0_1_SIGNATURE_VALUE


stage_2_start_16:
    cli
    mov         ax, cs
    mov         ds, ax
    mov         es, ax
    mov         sp, ax
    xor         ax, ax
    mov         ss, ax
    sti    

    mov     si, STR_STAGE_2_OK
    call    print_string_16
    call    hang_16


times (512*2-2)-($-$$)      db 0

STAGE_2_SIGNATURE           dw 0x1337
