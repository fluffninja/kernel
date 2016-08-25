ASM		:= nasm
ASMFLAGS	:= -f elf

CC 		:= gcc
CFLAGS 		:= -nostdinc -m32 -std=c11 -Wall -Wextra -c -I ../include \
		-fno-builtin -fno-stack-protector

LD 		:= ld
LDSCRIPT	:= kernel.ld
LDFLAGS		:= -T $(LDSCRIPT) -nostdlib -m elf_i386 -x -s 

.PHONY: all
all: $(LDSCRIPT) kernel.bin

%.o: %.asm
	$(ASM) $< -o $@ $(ASMFLAGS)

%.o: %.c
	$(CC) $< -o $@ $(CFLAGS)

kernel.bin: bootstrap.o kmain.o
	$(LD) $^ -o $@ $(LDFLAGS)	

kmain.c: ps2.h
