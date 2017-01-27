# Kernel

I'm making a kernel.

## Building and Running

Please double-check the prerequisites before trying anything :-)

To build and create a bootable floppy image, use `make image`.

To run once built, use `make run`.

## Prerequisites

* make
* gcc
* nasm
* dd - to create the floppy image

For quick emulation I recommend using QEMU - specifically qemu-system-i386 for 32-bit x86 emulation.
