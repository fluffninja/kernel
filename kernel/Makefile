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
kernel.elf: start.bin kmain.o con.o ps2.o pic.o cpu/idt.o cpu/isr.o \
	irq.o kb.o kio.o panic.o mouse.o vga.o cpu/syscall.o boot.o \
	klog.o asm/cpustat.o hexdump.o
	$(LD) $^ ../libc/libc.a -o $@ $(LDFLAGS) 1> $(LDMAP)

# Flatten Kernel ELF File
kernel.bin: kernel.elf
	objcopy -O binary -S $^ $@

kmain.c: boot.h con.h cpu/idt.h cpu/isr.h irq.h kb.h kio.h panic.h mouse.h \
	ps2.h vga.h cpu/syscall.h
con.c: con.h kio.h vga.h
ps2.c: ps2.h kio.h
pic.c: pic.h cpu/idt.h kio.h
cpu/idt.c: cpu/idt.h
cpu/isr.c: cpu/isr.h cpu/idt.h panic.h
irq.c: irq.h cpu/isr.h pic.h kio.h
kb.c: kb.h kio.h irq.h ps2.h con.h panic.h keymap-en-us
kio.c: kio.h con.h
panic.c: panic.h kio.h con.h
mouse.c: mouse.h irq.h ps2.h kio.h con.h
vga.c: vga.h kio.h
cpu/syscall.c: cpu/syscall.h cpu/isr.h panic.h kio.h
boot.c: boot.h
hexdump.c: kio.h
klog.c: kio.h
asm/cpustat.c: kio.h
