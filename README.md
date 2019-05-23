# Kernel

A basic bootloader and kernel for the 32-bit x86.

## Building and Running

To build and create a bootable floppy image, use `make image`.

To run once built, use `make run`.

### Prerequisites

* make
* gcc
* nasm
* dd - to create the floppy image

For quick emulation I recommend using QEMU - specifically qemu-system-i386 for 32-bit x86 emulation.
