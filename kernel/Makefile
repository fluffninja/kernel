ASM		:= nasm
ASMFLAGS	:= -f elf

CC		:= gcc
CFLAGS		:= -nostdinc -m32 -std=c11 -Wall -Wextra -c -I ../include \
		-fno-builtin -fno-stack-protector -masm=intel

LD		:= ld
LDSCRIPT	:= kernel.ld
LDMAP		:= kernel.map
LDFLAGS		:= -T $(LDSCRIPT) -M -nostdlib -m elf_i386 -x -s

%.bin: %.asm
	$(ASM) $< -o $@ $(ASMFLAGS)

%.o: %.c
	$(CC) $< -o $@ $(CFLAGS)

kernel.bin: bootstrap.bin kmain.o kstring.o con.o ps2.o pic.o idt.o isr.o \
	irq.o kb.o
	$(LD) *.o ../libc/libc.a -o $@ $(LDFLAGS) 1> $(LDMAP)

# Everyone needs kstring
%.c: kstring.h

kmain.c: con.h idt.h isr.h irq.h kb.h
kstring.c: kstring.h
con.c: con.h
ps2.c: ps2.h
pic.c: pic.h idt.h
idt.c: idt.h
isr.c: isr.h idt.h 
irq.c: irq.h isr.h pic.h
kb.c: kb.h irq.h ps2.h 

