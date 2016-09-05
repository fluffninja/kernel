# Kernel

I'm making a kernel.

## Building and Running

Double-check the prerequisites before trying anything :-).

To build and create a bootable floppy image, use `make image`.

To run, use `make run`.

To attempt to mount the image, use `make mount`. 
To unmount, use `make unmount`.
Note that this is dependent on the FAT header in boot\boot.asm.

## Prerequisites

The building process relies on some GNU coreutils, so a Linux build environment 
is recommended.

* nasm
* gcc
* qemu (to emulate)
* dd (to make the floppy image)

## Known Issues

* At the moment, the bootloader relies on extra BIOS functions to read from the
floppy drive. While standardised, they don't seem to be  widely supported - in
particular they are not supported by, of all things, VirtualBox or VMware.
Thankfully QEMU does support these functions.

* The bootloader currently doesn't activate the A20 (although the code is
present), because QEMU _helpfully_ enables it for us by default. Hence I 
cannot test it properly.

* Sometimes the primary makefile doesn't notice changes: use `make clean` to 
delete everything before trying to build again.

## Naming

*Just to make the codebase a little easier to understand...*

Duplications of C standard library functions within the kernel are prefixed
by the letter 'k' (e.g. `kmemset`, `kvsnprintf`). This is to help prevent 
confusion in the event that the functions behave slightly differently.

Functions prefixed by a double-underscore (e.g. `__irq_default_hook_func`) are
usually helper-functions in one way or another.
