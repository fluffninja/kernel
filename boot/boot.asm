;
;                      boot.asm - Bootloader for Isaac OS
;
; It does the following:
; * Stage 0 - Load stages 1 and 2 off disk.
; * Stage 1 - Switch to 32-bit protected mode.
; * Stage 2 - Jump to the kernel.
;
; Sounds easy... right?
;
; Specifically:
; 0) Load the sectors following the bootloader sector and place them straight
;    after this one in memory, then jump to the code contained within them.
; 1) Try to enter protected mode:
;    a) Enable the A20
;    b) Install the GDT and IDT
;    c) Enable protected mode
; 2) Load the rest of the kernel off-disk to somewhere above 1MB and jump to it
;
; Metrics:
; * The size of the real-mode physical address space is 2^20 = 0x100000 = 1MB
; * Of this, RAM is mapped to just over half:                  0x0A0000 = 640KB
; * The BIOS loads us at 0x7C00, 512B under 32K (32K = 0x8000) 0x007C00
; 
; Useful Resources:
; * http://x86.renejeschke.de/            - x86 Instruction Set Reference
; * https://en.wikipedia.org/wiki/INT_10H - Low-level Text/Video Services/IO
; * https://en.wikipedia.org/wiki/INT_13H - Low-level Disk Services/IO
; * https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html - FAT Stuff


; Absolute address to which the BIOS loads the first sector of the bootloader
%define STAGE_0_ADDRESS             (0x7c00)

; Address to which stage 0 will load stage 1+
%define STAGE_1_ADDRESS             (STAGE_0_ADDRESS + 512)

; Number of sectors to be read off-disk by stage 0
%define STAGE_0_READ_SECTOR_COUNT   (18)

; Absolute index on-disk of first sector to be loaded by stage 0
%define STAGE_0_READ_SECTOR_INDEX   (1)


    [bits   16]                     ; Stages 1 and 2 are 16-bit
    [org    STAGE_0_ADDRESS]        ; Offset added to all symbol addresses


    ; Jump over FAT header. We have room for no more than 3 bytes, so there's
    ; only room for a relative jump.
    jmp         short stage0_normalise_code_address_16


; Pad out so that the header always starts at the third byte
times (3 - ($ - $$))                db 0

; Fields for a standard FAT16 header
DRIVE_OEM                           db  "ISAAC OS"      ; 8 Characters
DRIVE_BYTES_PER_SECTOR              dw  512
DRIVE_SECTORS_PER_CLUSTER           db  1
DRIVE_RESERVED_SECTOR_COUNT         dw  2
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


stage0_normalise_code_address_16:
    ; The BIOS can place us at 07c0:0000 or 0000:7c00 -- the same address, but 
    ; addressed using different schemes. The following long jump normalises 
    ; things to the latter.
    jmp         long 0x0000:stage0_start_16


; Data Access Packet (DAP) - for use with the BIOS extended disk functionality.
; Currently set up and ready for use with stage 0, and later modified and 
; reused by stage 1.
data_access_packet_struct:
                        db  0x10                        ; Size of packet
                        db  0                           ; Reserved
.read_sector_count      dw  STAGE_0_READ_SECTOR_COUNT   ; Sectors to read
.destination_offset     dw  STAGE_1_ADDRESS             ; Destination offset
.destination_segment    dw  0                           ; Destination segment
.read_sector_index      dq  STAGE_0_READ_SECTOR_INDEX   ; On-disk sector index


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

    ; The BIOS has placed the boot device ID in the DL register

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

    ; Number of floppy read/reset attempts before quitting
    mov         di, 5

.read_floppy:
    ; Use the extended BIOS functions to load sectors off of the disk.
    ; Details of what to load are found in the disk address packed (DAP)
    ; - defined below.
    mov         ah, 0x42
    mov         si, data_access_packet_struct
    stc 
    int         0x13
    jc          .read_fail

    jmp         long 0x0000:stage1_start_16

.no_disk_extensions:
    mov         si, MSG_NO_DISK_EXTENSIONS
    call        print_line_16

    jmp         short .reboot

.read_fail:    
    ; Check attempt count. If zero, quit.
    dec         di
    jz          short .quit

    ; Reset floppy drive
    xor         ah, ah
    stc 
    int         0x13
    jnc         short .read_floppy

    ; If we get here, floppy reset failed.
    ; Fall-through to .quit

.quit:
    mov         si, MSG_NO_BOOT_DUE_TO_DISK
    call        print_line_16

    mov         si, MSG_DISK_ID
    mov         bl, dl
    call        print_string_hex_line_16

    ; Most INT 0x13 functions place error codes in AH
    mov         si, MSG_ERRROR_CODE
    mov         bl, ah
    call        print_string_hex_line_16

.reboot:
    ; Fall-through to reboot_16


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

%if 0
; Acquires details about the floppy from the BIOS
; IN:
;   b_boot_device
; OUT:
;   CF: On error
;   w_sectors_per_cylinder
;   w_head_count
get_floppy_details_16:
    pusha
    push        es                              ; Clob'd, not included by PUSHA 

    xor         ax, ax                          ; Zero ES:DI for some BIOSes
    mov         es, ax
    xor         di, di
    mov         dl, [b_boot_device]
    mov         ah, 0x08
    int         0x13
    jc          short .error                    ; Carry set on fail, code in AH

    movzx       dx, dh                          ; Index of last head
    inc         dx                              ; Increment to get count
    mov         [w_head_count], dx

    and         cx, 0x3f                        ; Deal with whacky encoding
    mov         [w_sectors_per_cylinder], cx

    ; The other goodies can be ignored for now, just end it
    jmp         short .done

.error: 
    mov         si, MSG_ERROR
    call        print_string_16
    call        print_hex_16                    ; Code is already in AH

.done:
    pop         es
    popa

    ret


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
logical_to_hcs_16:
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

    pop         ax                              ; Restore clobbered AX and BX
    pop         bx

    ret
%endif


; Error messages for our dear user
; Keep these brief, for we've only 512 bytes of space
MSG_NO_DISK_EXTENSIONS  db "BIOS unsupported", 0
MSG_ERRROR_CODE         db "Code ", 0
MSG_DISK_ID             db "Disk ", 0
MSG_NO_BOOT_DUE_TO_DISK db "Disk issue preventing boot", 0
MSG_KEY_TO_REBOOT       db "Press any key to reboot", 10, 13, 0


; Pad the rest of this sector with zeroes, appart from...
times ((512 - 2) - ($ - $$)) db 0


; ...the last word, which is the boot signature.
g_sector_0_signature_w  dw 0xaa55


; --- End of first sector (512 bytes) --- ;


; These can be read during kernel console initialisation so that future
; text output carries on from where it left off.
; They're at a known location: 0x7e00
g_cursor_pos_x_b        db 0
g_cursor_pos_y_b        db 0


; More strings
MSG_LOADING             db "Loading, please wait...", 0
MSG_NO_BOOT_DUE_TO_A20  db "A20 intialisation error", 0


; See https://www.win.tue.nl/~aeb/linux/kbd/A20.html
; See http://wiki.osdev.org/A20_Line
; See http://kernelx.weebly.com/a20-address-line.html
stage1_start_16:
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

    mov         ax, 0x2403
    int         0x15

.init_pm:
    cli

    ; Load the GDT and IDT descriptions - defined below
    lgdt        [gdt_description]
    lidt        [idt_description]

    ; Set the first bit of the machine status word to specify protected mode.
    ; Value is OR'd so as to not erase already-set bits.
    mov         eax, cr0
    or          eax, 0x1
    mov         cr0, eax

    jmp         0x08:stage2_start_32            ; Code segment is now 0x08 :-)

.quit:
    sti
    mov         si, MSG_NO_BOOT_DUE_TO_A20
    call        print_line_16
    call        reboot_16



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


; We're no in 32-bit code land.
; Load the new data segment from the GDT.
stage2_start_32:
    mov         ax, 0x10                        
    mov         ds, ax                          ; Data segment is now 0x10 :-)
    mov         es, ax
    mov         ss, ax
    mov         esp, 0x90000

    ; Jump to the kernel!
    jmp         0x7c00 + 512 * 2


; Pad out the rest of this sector 
times ((512 * 2) - ($ - $$)) db 0
