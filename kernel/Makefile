TargetBin := kernel.bin
TargetElf := kernel.elf
Dependencies := \
	start.bin \
	kmain.o \
	console.o \
	ps2.o \
	pic.o \
	irq.o \
	kb.o \
	kio.o \
	panic.o \
	mouse.o \
	vga.o \
	boot.o \
	klog.o \
	hexdump.o \
	kerror.o \
	cpu/gdt.o \
	cpu/syscall.o \
	cpu/idt.o \
	cpu/isr.o \
	asm/cpustat.o \
	../libc/libc.a

## C Compiler Options ##
CC := gcc
CFLAGS := -c
CFLAGS += -std=c11 -masm=intel
CFLAGS += -nostdinc -I ../include
CFLAGS += -m32
CFLAGS += -mno-red-zone
CFLAGS += -fno-builtin
CFLAGS += -fno-stack-protector
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -fno-combine-stack-adjustments
CFLAGS += -Wall -Wextra
CFLAGS += -Wno-implicit-fallthrough

## Assembler Options ##
AS := nasm

## Linker Options ##
LinkerScript := kernel.ld
LinkerMap := kernel.map
LD := ld
LDFLAGS := -m elf_i386
LDFLAGS += -T $(LinkerScript)
LDFLAGS += -nostdlib
LDFLAGS += -M >$(LinkerMap)

## Recipes ###

%.bin: %.asm
	$(AS) -f elf $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(TargetElf) $(LinkerMap): $(LinkerScript) $(Dependencies)
	$(LD) $(LDFLAGS) $(Dependencies) -o $(TargetElf)

$(TargetBin): $(TargetElf)
	objcopy -O binary -S $< $@


## Header-File Dependencies ##

asm/cpustat.c: kio.h

cpu/idt.c: cpu/idt.h
cpu/isr.c: cpu/isr.h cpu/idt.h panic.h
cpu/gdt.c: cpu/gdt.h
cpu/syscall.c: cpu/syscall.h cpu/isr.h panic.h kio.h

kmain.c: boot.h console.h cpu/idt.h cpu/isr.h irq.h kb.h kio.h panic.h mouse.h \
	ps2.h vga.h cpu/syscall.h

boot.c: boot.h
console.c: console.h vga.h
hexdump.c: kio.h
irq.c: irq.h cpu/isr.h pic.h
kb.c: kb.h irq.h ps2.h console.h panic.h keymap-en-us
kio.c: kio.h console.h
klog.c: kio.h
mouse.c: mouse.h irq.h ps2.h console.h
panic.c: panic.h kio.h console.h
pic.c: pic.h cpu/idt.h
ps2.c: ps2.h
vga.c: vga.h
