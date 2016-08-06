%define STAGE_0_1_SIGNATURE_VALUE   0xaa55
%define STAGE_0_1_SIZE              512
%define STAGE_0_SEGMENT             0x07c0
%define STAGE_1_SEGMENT             0x8000
%define PADDING_VALUE               hlt
%define STAGE_2_FILENAME            "BOOT    BIN"


    [bits   16]
    [org    0]


    jmp         stage_0_start_16

    ; Ensure that the FAT header is 3 bytes in
times ((3) - ($ - $$))              PADDING_VALUE


    ; Fields for a standard FAT16 header
DRIVE_OEM                           db  "ISAAC OS"      ; 8 Characters
DRIVE_BYTES_PER_SECTOR              dw  512
DRIVE_SECTORS_PER_CLUSTER           db  1
DRIVE_RESERVED_SECTOR_COUNT         dw  1
DRIVE_FAT_COUNT                     db  2
DRIVE_ROOT_ENTRY_COUNT              dw  224
DRIVE_FILESYSTEM_SECTOR_COUNT       dw  2880
DRIVE_MEDIA_DESCRIPTOR              db  0xf0
DRIVE_SECTORS_PER_FAT               dw  9
DRIVE_SECTORS_PER_TRACK             dw  12
DRIVE_HEAD_COUNT                    dw  1
DRIVE_HIDDEN_SECTOR_COUNT           dd  0
DRIVE_FILESYSTEM_SECTOR_COUNT_32    dd  0               ; TODO
DRIVE_LOGICAL_DRIVE_NUMBER          db  0
                                    db  0               ; Reserved
DRIVE_EXTENDED_SIGNATURE            db  0x29
DRIVE_VOLUME_SERIAL_NUMBER          dd  0x00000000
DRIVE_VOLUME_LABEL                  db  "OS BOOTDISK"   ; 11 Characters
DRIVE_VOLUME_FILESYSTEM_TYPE        db  "FAT16   "      ; 8 Characters


MSG_STAGE_0_FATAL                   db  'X'

MSG_STAGE_1_OK                      db  "OK", 0
MSG_HALT                            db  "HALT", 0

STR_STAGE_2_FILENAME                db  STAGE_2_FILENAME, 0

DIGIT_VALUES                        db  "0123456789ABCDEF"

b_boot_device                       db  0


stage_0_start_16:
    ; Given to us by the BIOS; save it for later.
    mov         [b_boot_device], dl 

    ; Check the bootloader signature prior to moving.
    ; If this check fails then the BIOS did something very wrongly.
    mov         ax, STAGE_0_SEGMENT
    mov         ds, ax
    mov         ax, [STAGE_0_1_SIGNATURE]
    cmp         ax, STAGE_0_1_SIGNATURE_VALUE
    jne         short .fail

    ; Copy the whole bootloader segment elsewhere
    mov         ax, STAGE_1_SEGMENT
    mov         es, ax
    xor         si, si
    xor         di, di
    mov         cx, (STAGE_0_1_SIZE / 2)            ; Divide by 2 because words
    cld
    rep movsw    

    ; Check the bootloader signature after moving.
    mov         ax, STAGE_1_SEGMENT
    mov         ds, ax
    mov         ax, [STAGE_0_1_SIGNATURE]
    cmp         ax, STAGE_0_1_SIGNATURE_VALUE
    jne         short .fail

    ; Jump to the copy of the code in the new location
    jmp         long STAGE_1_SEGMENT:stage_1_start_16

.fail:
    ; Something went badly wrong in what should have been a straight-forward
    ; operation, and we cannot continue. Providing the BIOS isn't screwed,
    ; attempt to print something to the screen, wait for keyboard input, and
    ; then (soft) reboot.
    mov         bx, 0x0007
    mov         ah, 0x0e
    mov         al, MSG_STAGE_0_FATAL
    int         0x10
    xor         ax, ax
    int         0x16
    int         0x19


stage_1_start_16:
    ; Set up the segment registers and the stack. No interrupts during this...
    cli
    mov         ax, cs
    mov         ds, ax
    mov         es, ax
    mov         sp, ax
    xor         ax, ax
    mov         ss, ax
    sti    

    mov         si, MSG_STAGE_1_OK
    call        print_line_16
 
    call        hang_16


; Print a hexadecimal byte value
;   AL: Byte value
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


; Moves the text cursor to the beginning of the next line
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
;   SI: String Address
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


; Print a null-terminated string followed by a new-line
;   SI: String Address
print_line_16:
    call        print_string_16
    call        move_to_new_line_16
    ret


; Print a message to the screen and halt the CPU
hang_16:
    mov         si, MSG_HALT
    call        print_line_16
    hlt
    jmp         $


; Convert a logical sector number into a head, track, and sector value as used
; by the BIOS services. The output values are ready for use with INT 0x13.
; IN:
;   AX: Logical Sector (from 0)
; OUT:
;   CL: Sector
;   CH: Track
;   DH: Head
sector_to_hts_16:
    push        bx
    push        ax

    mov         bx, ax                          ; Save for later use

    xor         dx, dx                          ; DIV remainder goes into DX
    div         word [DRIVE_SECTORS_PER_TRACK]  ; DIV AX
    inc         dl                              ; HTS sector no. counts from 1
    mov         cl, dl

    mov         ax, bx                          ; Retrieve save value
    div         word [DRIVE_SECTORS_PER_TRACK]  ; DIV AX (don't need remainder)
    xor         dx, dx                          ; Ignore first remainder
    div         word [DRIVE_HEAD_COUNT]
    mov         ch, al                          ; Quotient is track no.
    mov         dh, dl                          ; Remainder is head no.

    pop         ax
    pop         bx

    ret


times ((512 - 2) - ($ - $$))        PADDING_VALUE
STAGE_0_1_SIGNATURE                 dw  STAGE_0_1_SIGNATURE_VALUE
