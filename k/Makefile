%.bin: %.asm
	@nasm $< -o $@ -f bin

boot.img: foo.bin bar.bin
	@rm -fv $@
	@dd of=$@ if=/dev/zero bs=1474560 count=1
	@dd of=$@ if=foo.bin bs=512 count=2 seek=0 conv=notrunc
	@dd of=$@ if=bar.bin bs=512 count=2 seek=2 conv=notrunc

.PHONY: clean
clean:
	@rm -frv *.img *.bin *.o

.PHONY: run
run: boot.img
	qemu-system-i386 -monitor stdio -k en-gb -m 16M \
		-drive media=disk,format=raw,file=$<
