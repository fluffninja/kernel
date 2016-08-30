ASM		:= nasm
ASMFLAGS	:= -f elf

CC		:= gcc
CFLAGS		:= -nostdinc -m32 -std=c11 -Wall -Wextra -c -I ../include \
		-fno-builtin -fno-stack-protector -masm=intel

LD		:= ld
LDSCRIPT	:= kernel.ld
LDFLAGS		:= -T $(LDSCRIPT) -nostdlib -m elf_i386 -x -s

.PHONY: all
all: kernel.bin

%.o: %.asm
	$(ASM) $< -o $@ $(ASMFLAGS)

%.o: %.c
	$(CC) $< -o $@ $(CFLAGS)

kernel.bin: bootstrap.o kmain.o kstring.o con.o ps2.o pic.o
	$(LD) $^ -o $@ ../libc/libc.a $(LDFLAGS)

kmain.c: kstring.h ps2.h con.h kstring.h
kstring.c: kstring.h
con.c: con.h kstring.h
ps2.c: ps2.h kstring.h
pic.c: pic.h kstring.h
