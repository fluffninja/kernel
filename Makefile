boot.bin: boot.asm
	nasm $< -o $@ -f bin

boot.img: boot.bin
	dd if=$< of=$@ bs=512 count=1

.PHONY: run
run: boot.img
	qemu-system-i386 -monitor stdio -k en-gb -m 128M -drive \
		media=disk,format=raw,file=boot.img
