# Kernel

I'm making a kernel.

## Building and Running

Double-check the prerequisites before trying anything :-).

To build and create a bootable floppy image, use `make image`.

To run, use `make run`.

To attempt to mount the image, use `make mount`. 
To unmount, use `make unmount`.
Note that this is dependent on the FAT header in boot/boot.asm.

## Prerequisites

The building process relies on some GNU coreutils, so a Linux build environment 
is recommended.

* nasm
* gcc
* qemu (to emulate)
* dd (to make the floppy image and write to it)

## Known Issues

* Sometimes the primary makefile doesn't notice changes: use `make clean` to 
delete everything before trying to build again.
