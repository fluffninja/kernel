;
; boot.asm - Bootloader
;
; It does the following:
; * Stage 1 - Load Stage 2 and the kernel image off disk, jump to stage 2.
; * Stage 2 - Switch to 32-bit mode, move kernel image to 1MB, jump to kernel.
;
; Sounds easy... right?
;
; Metrics:
;  Real-mode Physical Address Space Size: 2^20 = 10 0000 = 1MB
;  Real-mode Accessible RAM Size:                 A 0000 = 640KB
;  Stage 2 Location (Loaded by BIOS):               7C00 = 32KB - 512B * 2
;  Stage 2 Location (Loaded by stage 1):            7E00 = 32KB - 512B
;  Kernel Image Location (Loaded by Stage 1 too):   8000 = 32KB
;  Kernel Target Location (Moved by Stage 2):    10 0000 = 1MB
;  Stack for Stages 1 and 2:                        7C00 (Grows Down)
;  Stack for Kernel (Set by Stage 2):             A 0000 (Grows Down)
;  Disk Sector Size:                                0200 = 512B
; Note that memory below 0500 is unusable, as it consists of the IVT and BIOS
; Data Area (BDA).
; Note that the Extended BIOS Data Area (EBDA) sometimes sits below A 0000.
; It's never more than 8KB (0400) in size.
;
; Useful Resources:
; * http://x86.renejeschke.de/            - x86 Instruction Set Reference
; * https://en.wikipedia.org/wiki/INT_10H - Low-level Text/Video Services/IO
; * https://en.wikipedia.org/wiki/INT_13H - Low-level Disk Services/IO
; * https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html - FAT Stuff


; Codification of the above metrics
%define STAGE_1_LOCATION            (0x7c00)
%define STAGE_1_SIZE                (512)
%define STAGE_1_SECTORS             (STAGE_1_SIZE / 512)
%define STAGE_2_LOCATION            (STAGE_1_LOCATION + STAGE_1_SIZE)
%define STAGE_2_SIZE                (512)
%define STAGE_2_SECTORS             (STAGE_2_SIZE / 512)
%define KERNEL_IMAGE_LOCATION       (STAGE_2_LOCATION + STAGE_2_SIZE)
%define KERNEL_IMAGE_SIZE           (0x010000 - KERNEL_IMAGE_LOCATION)
%define KERNEL_IMAGE_SECTORS        (KERNEL_IMAGE_SIZE / 512)
%define STAGE_1_PAYLOAD_START       (STAGE_1_SECTORS)
%define STAGE_1_PAYLOAD_SECTORS     (STAGE_2_SECTORS + KERNEL_IMAGE_SECTORS)
%define STAGE_1_SIGNATURE           (0xaa55)
%define KERNEL_TARGET_LOCATION      (0x100000)
%define UNINITIALISED_LOCATION      (0x7a00)


    
    ; These first few parts are 16-bit. We'll switch to 32-bit later on.
    [bits       16]


    [org        STAGE_1_LOCATION]


    ; Jump over FAT header. 3 bytes are allowed before the header, which is
    ; only enough room for a relative jump.
    jmp         short stage_1_normalise_code_address_16


; Pad up to the third byte
times (3 - ($ - $$))                db 0


; Fields for a standard FAT16 header
DRIVE_OEM                           db  "FLUFF OS"      ; 8 Characters
DRIVE_BYTES_PER_SECTOR              dw  512
DRIVE_SECTORS_PER_CLUSTER           db  1
DRIVE_RESERVED_SECTOR_COUNT         dw  (STAGE_1_SECTORS + STAGE_2_SECTORS)
DRIVE_FAT_COUNT                     db  0               ; TODO
DRIVE_ROOT_ENTRY_COUNT              dw  0               ; TODO
DRIVE_FILESYSTEM_SECTOR_COUNT       dw  0               ; TODO
DRIVE_MEDIA_DESCRIPTOR              db  0xf0            ; 3.5" Flopsy
DRIVE_SECTORS_PER_FAT               dw  0               ; TODO
DRIVE_SECTORS_PER_CYLINDER          dw  0               ; TODO
DRIVE_HEAD_COUNT                    dw  1
DRIVE_HIDDEN_SECTOR_COUNT           dd  0
DRIVE_FILESYSTEM_SECTOR_COUNT_32    dd  0               ; TODO
DRIVE_LOGICAL_DRIVE_NUMBER          db  0
                                    db  0               ; Reserved
DRIVE_EXTENDED_SIGNATURE            db  0x29            ; 3 More Fields Present
DRIVE_VOLUME_SERIAL_NUMBER          dd  0x1544c051      ; Arbritrary
DRIVE_VOLUME_LABEL                  db  "OS BOOTDISK"   ; 11 Characters
DRIVE_VOLUME_FILESYSTEM_TYPE        db  "FAT16   "      ; 8 Characters  


stage_1_normalise_code_address_16:
    ; The BIOS can place us at 07c0:0000 or 0000:7c00 -- the same address, but 
    ; addressed using different schemes. The following long jump normalises 
    ; things to the latter.
    jmp         long 0x0000:stage_1_start_16


stage_1_start_16:
    ; Set up segments and stack - don't allow interrupts whilst doing this.
    cli

    ; Data, extra, and stack segments will start at zero.
    xor         ax, ax
    mov         ds, ax
    mov         es, ax
    mov         ss, ax

    ; The stack will grow downwards from beneath our bootloader.
    mov         sp, STAGE_1_LOCATION

    sti

    ; The BIOS has given us the ID of the boot device in DL.
    ; Save it for later use if necessary.
    mov         byte [b_boot_device], dl

    ; Read parameters of the disk
    xor         di, di
    mov         es, di
    mov         ah, 0x08
    int         0x13
    jc          floppy_read_error_16

    inc         dh
    movzx       dx, dh
    mov         word [w_head_count], dx

    and         cl, 0x3f
    movzx       cx, cl
    mov         word [w_sectors_per_cylinder], cx

    ; Try to detect whethere this BIOS supports the floppy extension functions.
    mov         ah, 0x41
    mov         bx, 0x55aa
    xor         cx, cx
    stc                                     ; Carry must be explicitly unset
    int         0x13

    ; Must pass all three checks to verify presence of extensions
    jc          short .no_disk_extensions
    cmp         bx, 0xaa55
    jne         short .no_disk_extensions
    test        cx, 0x0001                  ; Bit 1 signifies DAP capabilities
    jz          short .no_disk_extensions

    ; If all tests passed, load the floppy using the extended functions
    call        load_floppy_using_extensions_16

.no_disk_extensions:
    mov         ax, STAGE_1_PAYLOAD_START
    mov         bx, STAGE_2_LOCATION
    mov         cx, STAGE_1_PAYLOAD_SECTORS    

.read_floppy:
    pusha
    call        lba_to_hcs_16
    mov         ah, 0x02
    mov         al, 1
    int         0x13
    jc          short .read_fail
    popa

    add         bx, 512
    inc         ax

    dec         cx
    test        cx, cx
    jz          short .done

    jmp         short .read_floppy

.done:
    jmp         long 0x0000:stage_2_start_16

.read_fail:
    call        floppy_read_error_16


load_floppy_using_extensions_16:
    ; Number of floppy read/reset attempts before quitting
    mov         di, 5

.read_floppy:
    ; Use the extended BIOS functions to load sectors off of the disk.
    ; Details of what to load are found in the disk address packed (DAP)
    ; - defined below.
    mov         ah, 0x42
    mov         si, .data_access_packet_struct
    stc 
    int         0x13
    jc          .read_fail

    jmp         long 0x0000:stage_2_start_16

    ; Data Access Packet- for use with the BIOS extended disk functionality.
    ; Currently set up and ready for use with stage 1, and later modified and 
    ; reused by stage 2.
.data_access_packet_struct:
                        db  0x10                        ; Size of packet
                        db  0                           ; Reserved
.read_sector_count      dw  STAGE_1_PAYLOAD_SECTORS     ; Sectors to read
.destination_offset     dw  STAGE_2_LOCATION            ; Destination offset
.destination_segment    dw  0                           ; Destination segment
.read_sector_index      dq  STAGE_1_PAYLOAD_START       ; On-disk sector index

.read_fail:    
    ; Check attempt count. If zero, quit.
    dec         di
    call        floppy_read_error_16

    ; Reset floppy drive
    xor         ah, ah
    stc 
    int         0x13
    jnc         short .read_floppy

    ; If we get here, floppy reset failed.
    ; Fall-through to floppy_read_error_16

floppy_read_error_16:
    mov         si, MSG_NO_BOOT_DUE_TO_DISK
    call        print_line_16

    mov         si, MSG_DISK_ID
    mov         bl, dl
    call        print_string_hex_line_16

    ; Most INT 0x13 functions place error codes in AH
    mov         si, MSG_ERRROR_CODE
    mov         bl, ah
    call        print_string_hex_line_16


reboot_16:
    mov         si, MSG_KEY_TO_REBOOT
    call        print_line_16

    xor         ax, ax
    int         0x16        ; Wait for a key-press
    int         0x19        ; Warm reboot

    ; If the above interrupts fail, just halt
    cli    
    hlt


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
    ; To make things easier, we're just going to print a line-feed
    ; followed by a carriage-return.
    pusha

    mov         ah, 0x0e
    xor         bh, bh

    mov         al, 10
    int         0x10
    mov         al, 13
    int         0x10

    popa
    ret


; Print a null-terminated string to the screen and move down to the next line
; IN:
;   SI: String segment offset address
print_line_16:
    call        print_string_16
    call        print_new_line_16
    ret


; Print a null-terminated string to the screen followed by a hexadecimal byte,
; and move the cursor to the next line
; IN:
;   SI: String segment offset address
;   BL: Value to convert and print as hex
print_string_hex_line_16:
    call        print_string_16
    call        print_hex_byte_16
    call        print_new_line_16
    ret


; Print a hexadecimal value to the screen (limited to bytes only)
; IN:
;   BL: Value to convert and print as hex
print_hex_byte_16:
    pusha

    mov         ah, 0x0e
    xor         bh, bh

    ; Get the first nibble
    mov         al, bl
    shr         al, 4
    movzx       si, al
    mov         al, [si + .HEX_DIGITS]
    int         0x10

    ; Get the second nibble
    mov         al, bl
    and         al, 0x0f
    movzx       si, al
    mov         al, [si + .HEX_DIGITS]
    int         0x10

    popa
    ret

    .HEX_DIGITS db "0123456789ABCDEF"


; Converts an index for an absolute logical sector (were all sectors simply
; indexed from 0) to the head-cylinder(/track)-sector system used by the BIOS.
; IN:
;   AX: Logical sector index (zero-based)
; OUT:
;   (Registers ready for use by INT 0x13, other than AX and ES:BX)
;   CH: Cylinder
;   CL: Sector
;   DH: Head
;   DL: Drive ID
lba_to_hcs_16:
    push        ax
    push        bx

    mov         bx, ax                          ; Save AX

    ; Get the actual sector value
    xor         dx, dx                          ; Part of operand, make sure 0
    div         word [w_sectors_per_cylinder]   
    inc         dl                              ; Remainder is sector, indexed
    mov         cl, dl                          ; from 1

    mov         ax, bx                          ; Restore saved AX

    ; Get the cylinder and head
    xor         dx, dx                          ; Part of operand, make sure 0
    div         word [w_sectors_per_cylinder]
    xor         dx, dx                          ; Part of operand, make sure 0
    div         word [w_head_count]
    mov         ch, al                          ; Quotient is cylinder
    mov         dh, dl                          ; Remainder is head

    mov         dl, byte [b_boot_device]        ; For INT 0x13, the drive ID

    pop         bx
    pop         ax                              ; Restore clobbered AX and BX

    ret


; Error messages for our dear user
; Keep these brief, for we've only 512 bytes of space                           
MSG_NO_BOOT_DUE_TO_DISK db "Cannot boot due to a problem with the disk", 0
MSG_KEY_TO_REBOOT       db "Press any key to reboot", 10, 13, 0
MSG_DISK_ID             db "Disk:  ", 0
MSG_ERRROR_CODE         db "Error: ", 0


; Pad the rest of this sector with zeroes (leave room for signature)
times ((512 - 2) - ($ - $$)) db 0


; Boot sector signature
g_sector_0_signature_w  dw STAGE_1_SIGNATURE


; --- End of first sector (512 bytes) --- ;


; These can be read during kernel console initialisation so that future
; text output carries on from where it left off.
; They're at a known location: 0x7e00 and 0x7e01
g_cursor_pos_x_b        db 0
g_cursor_pos_y_b        db 0
b_a20_init_attempts     db 5


; More strings
MSG_LOADING             db "Loading, please wait...", 0
MSG_NO_BOOT_DUE_TO_A20  db "A20 not initialised", 0


; See https://www.win.tue.nl/~aeb/linux/kbd/A20.html
; See http://wiki.osdev.org/A20_Line
; See http://kernelx.weebly.com/a20-address-line.html
stage_2_start_16:
    mov         si, MSG_LOADING
    call        print_line_16

    ; Save the position of the cursor, for continuity in later console output
    mov         ah, 0x03
    xor         bh, bh
    int         0x10
    mov         [g_cursor_pos_x_b], dl
    mov         [g_cursor_pos_y_b], dh

    ; Hide the blinking cursor
    mov         ah, 0x01
    mov         cx, 0x2100
    int         0x10


test_a20:
    ; Test the status of A20 by detecting memory wrap-around. We'll do this by
    ; testing whether we can wraparound to the bootloader sector signature.
    push        es
    mov         ax, 0xffff
    mov         es, ax
    mov         ax, word [es:(g_sector_0_signature_w + 0x10)]
    cmp         ax, STAGE_1_SIGNATURE
    pop         es

    ; If values aren't equal, then there's no memory wrap-around and the A20's
    ; enabled.
    jne         short init_pm

    ; TODO: Utilise more a20 init methods
    call        enable_a20_bios_16

    mov         ax, [b_a20_init_attempts]
    dec         ax
    mov         [b_a20_init_attempts], ax

    jnz         test_a20

    mov         si, MSG_NO_BOOT_DUE_TO_A20
    call        print_line_16
    call        reboot_16
    

init_pm:
    cli

    ; Load the GDT and IDT descriptions - defined below
    lgdt        [gdt_description]
    lidt        [idt_description]

    ; Set the first bit of the machine status word to specify protected mode.
    ; Value is OR'd so as to not erase already-set bits.
    mov         eax, cr0
    or          eax, 0x1
    mov         cr0, eax

    jmp         0x08:stage_2_start_32            ; Code segment is now 0x08 :-)


clear_kb_cmd_queue:
    push        ax
.repeat:
    in          al, 0x64
    test        al, 0x02
    jnz         short .repeat
    pop         ax
    ret


enable_a20_kb_16:
    push        ax
    mov         al, 0xdd
    out         0x64, al
    pop         ax
    ret


enable_a20_bios_16:
    pusha

    mov         ax, 0x2403      ; Detect presence of BIOS A20 functionality
    stc
    int         0x15
    jc          short .done     ; Function failed - not present
    test        ah, ah
    jnz         short .done     ; Ditto.

    mov         ax, 0x2402      ; Check status - result is ignored, more of a
    stc                         ; verification that functionality is present
    int         0x15
    jc          short .done     ; Function failed - something's fishy, quit.
    test        ah, ah
    jnz         short .done     ; Ditto.

    mov         ax, 0x2401      ; Enable A20 (even if it already is enabled)
    stc
    int         0x15            ; Doesn't really matter whether this succeeds

.done:
    popa
    ret


; Attempt to enable the A20 using port 0x92
; This is dangerous, as it can conflict with other devices
; Should be used as a last-resort
enable_a20_fast_16:
    push        ax
    in          al, 0x92
    test        al, 0x02
    jz          short .done     ; Only output if we have to make changes
    or          al, 0x02
    and         al, ~0x01       ; Disable bit 1, which is used to fast-reset
    out         0x92, al
.done:
    pop         ax
    ret


; GDT Entry Macro
; Defines a GDT entry, correctly arranging the fields into twisted, evil forms.
; Params: 
;   Base   (32 bits)
;   Limit  (20 bits)
;   Access (8  bits)
;   Flags  (4  bits)
%macro gdt_entry_m 4                                  ; FIELD | BITS
    dw ((%2) & 0xffff)                                ; Limit  00:16
    dw ((%1) & 0xffff)                                ; Base   00:16
    db (((%1) >> 16) & 0xff)                          ; Base   16:24
    db ((%3) & 0xff)                                  ; Access 00:08
    db ((((%4) & 0x0f) << 4) | (((%2) >> 16) & 0x0f)) ; Limit  16:20, Flags 0:4
    db ((%1) >> 24)                                   ; Base   24:32
%endmacro


; Utilise the above macro to define our Global Descriptor table. This will
; later get re-written by the kernel, but we need one for the time being 
; so that we can switch into protected mode.
; For now our GDT specifies a data segment and a code (text) segment. They
; both span the entirity of the virtual address space, thus specifying that
; there is no distinction between code and data in memory.
gdt:
.start:
    gdt_entry_m 0, 0, 0, 0                      ; Dummy Entry (must be present)
    gdt_entry_m 0, 0xfffff, 0b10011010, 0b1100  ; Text Segment (0x08)
    gdt_entry_m 0, 0xfffff, 0b10010010, 0b1100  ; Data Segment (0x10)
.end:


; The GDT description structure. 
gdt_description:
    dw (gdt.end - gdt.start - 1)                ; GDT Size
    dd gdt                                      ; GDT Address (32-bit)


; The IDT descritpion structure.
; We don't have or need one right now - there are no interrupts.
; The kernel will create this this, along with the proper GDT, later.
idt_description:
    dw 0                                        ; IDT Size
    dd 0                                        ; IDT Address (32-bit)


    ; 32-bit from here on out!
    [bits 32]


; We're now in 32-bit land.
; Load the new data segment from the GDT.
stage_2_start_32:
    mov         ax, 0x10                        
    mov         ds, ax                          ; Data segment is now 0x10 :-)
    mov         es, ax
    mov         ss, ax
    mov         esp, 0xA0000                    ; Grow downward from 640KB

    ; Move the kernel to its target location (Above 1MB)
    cld
    mov         esi, KERNEL_IMAGE_LOCATION
    mov         edi, KERNEL_TARGET_LOCATION
    mov         ecx, KERNEL_IMAGE_SIZE
    rep movsd

    ; Jump to the kernel!
    jmp         KERNEL_TARGET_LOCATION


; Pad out the rest of this sector 
times ((512 * 2) - ($ - $$)) db 0

    ; The following data definitions are uninitialised, and will not be
    ; present or use up space in the floppy image
    [absolute   UNINITIALISED_LOCATION]

b_boot_device                       resb 1
w_sectors_per_cylinder              resw 1
w_head_count                        resw 1
