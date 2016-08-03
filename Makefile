boot.bin: boot.asm
	nasm $< -o $@ -f bin

boot.img: boot.bin
	@rm -fv $@
	@dd if=/dev/zero of=$@ bs=1474560 count=1
	@dd if=$< of=$@ bs=512 count=1 seek=0 conv=notrunc

.PHONY: mount
mount: boot.img
	@if [ -d ./boot_disk/ ]; then \
		echo "Already mounted. If this is wrong, try 'make unmount'."; \
	else \
		mkdir -v ./boot_disk/; \
		sudo mount -v -t msdos -o loop boot.img ./boot_disk/; \
	fi

.PHONY: unmount
unmount:
	@if [ -d ./boot_disk/ ]; then \
		sudo umount -v ./boot_disk; \
		rm -frv ./boot_disk; \
	else \
		echo "Not mounted"; \
	fi

.PHONY: clean
clean:
	@rm -frv *.img *.bin

.PHONY: run
run: boot.img
	qemu-system-i386 -monitor stdio -k en-gb -m 128M -drive \
		media=disk,format=raw,file=boot.img
