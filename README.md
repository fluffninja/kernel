# Basic Kernel

This is a very basic bootloader and kernel for the 32-bit x86 that I made when
first learning about OS and kernel development. It's not very good, but it may
perhaps still be of interest to anyone who is starting out with this sort of
thing.

I am no longer actively developing this project.


## Building and Running

### Prerequisites

* make
* gcc
* nasm
* dd
* qemu-system-i386 (for running)


### Building

To build everything and create a bootable floppy image, use `make image`.


### Running

To run, after building, use `make run`.
