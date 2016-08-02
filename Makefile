EMU			:= qemu-system-i386
EMUFLAGS	:= -monitor stdio -m 128M -k en-gb
CC			:= gcc
CFLAGS		:= -ffreestanding -std=c11 -Wall -Wextra
ASM 		:= nasm
ASMFLAGS	:= -f bin

%.bin: %.asm
	$(ASM) $< -o $@ $(ASMFLAGS)

%.flp: %.bin
	dd if=$< of=$@ bs=512

.PHONY: run
run: boot.flp
	$(EMU) -drive file=$<,media=disk,format=raw $(EMUFLAGS)
