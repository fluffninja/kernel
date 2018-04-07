ASM		:= nasm
ASMFLAGS	:= -f elf

CC		:= gcc
CFLAGS		:= -std=c11 -c -I ../include \
		-Wall -Wextra \
		-m32 -masm=intel \
		-nostdinc -fno-builtin -fno-stack-protector \
		-mno-red-zone \
		-fno-omit-frame-pointer -fno-combine-stack-adjustments

LD		:= ld
LDSCRIPT	:= kernel.ld
LDMAP		:= kernel.map
LDFLAGS		:= -T $(LDSCRIPT) -M -nostdlib -m elf_i386

# Asm Source -> Bin (ELF Object) Files
%.bin: %.asm
	$(ASM) $< -o $@ $(ASMFLAGS)

# C Source -> ELF Object Files
%.o: %.c
	$(CC) $< -o $@ $(CFLAGS)

# Kernel ELF File
kernel.elf: start.bin kmain.o con.o ps2.o pic.o pit.o pit.bin cpu/idt.o cpu/isr.o \
	irq.o kb.o kio.o panic.o mouse.o vga.o cpu/syscall.o boot.o \
	klog.o asm/cpustat.o hexdump.o kerror.o cpu/gdt.o mem/heap.o \
	mem/page.o mem/page.bin
	$(LD) $^ ../libc/libc.a -o $@ $(LDFLAGS) 1> $(LDMAP)

# Flatten Kernel ELF File
kernel.bin: kernel.elf
	objcopy -O binary -S $^ $@

# Private Header Dependencies

# asm
asm/cpustat.c: kio.h

# cpu
cpu/idt.c: cpu/idt.h
cpu/isr.c: cpu/isr.h cpu/idt.h panic.h
cpu/gdt.c: cpu/gdt.h
cpu/syscall.c: cpu/syscall.h cpu/isr.h panic.h kio.h

# mem
mem/heap.c: mem/heap.h

# init \ kmain
kmain.c: boot.h con.h cpu/idt.h cpu/isr.h irq.h kb.h kio.h panic.h mouse.h \
	ps2.h vga.h cpu/syscall.h mem/page.h

# components
boot.c: boot.h
con.c: con.h vga.h
hexdump.c: kio.h
irq.c: irq.h cpu/isr.h pic.h
kb.c: kb.h irq.h ps2.h con.h panic.h keymap-en-us
kio.c: kio.h con.h
klog.c: kio.h
mouse.c: mouse.h irq.h ps2.h con.h
panic.c: panic.h kio.h con.h
pic.c: pic.h cpu/idt.h
pit.c: pit.h pit.asm
ps2.c: ps2.h
vga.c: vga.h
mem/page.c: mem/page.h kio.h