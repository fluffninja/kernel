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

%.bin: %.asm
	$(ASM) $< -o $@ $(ASMFLAGS)

%.o: %.c
	$(CC) $< -o $@ $(CFLAGS)

kernel.elf: bootstrap.bin kmain.o kstring.o con.o ps2.o pic.o idt.o isr.o \
	irq.o kb.o kio.o panic.o mouse.o vga.o syscall.o boot.o
	$(LD) $^ ../libc/libc.a -o $@ $(LDFLAGS) 1> $(LDMAP)

kernel.bin: kernel.elf
	objcopy -O binary -S $^ $@

kmain.c: con.h idt.h isr.h irq.h kb.h mouse.h boot.h
kstring.c: kstring.h
con.c: con.h kio.h kutil.h boot.h
ps2.c: ps2.h kio.h
pic.c: pic.h idt.h kio.h
idt.c: idt.h kio.h kutil.h
isr.c: isr.h idt.h kio.h
irq.c: irq.h isr.h pic.h kio.h kutil.h
kb.c: kb.h irq.h ps2.h kio.h
kio.c: kio.h con.h kutil.h kstring.h
panic.c: panic.h kio.h
mouse.c: mouse.h irq.h ps2.h kio.h
vga.c: vga.h kio.h
syscall.c: syscall.h isr.h panic.h kio.h
boot.c: boot.h
