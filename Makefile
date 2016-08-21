OUTPUT_DIR 			:= out
OUTPUT_IMAGE 		:= $(OUTPUT_DIR)/bootdisk.img
OUTPUT_IMAGE_SIZE	:= 1474560

MOUNT_DIR			:= $(OUTPUT_DIR)/bootdisk_mout/

KERNEL_LINK_SCRIPT  := kernel/kernel.ld

ASM 				:= nasm
ASMFLAGS 			:= -f bin 

CC 					:= gcc
CFLAGS 				:= -nostdinc -nostdlib -ffreestanding -fno-builtin -m32 \
						-std=c11 -Wall -Wextra -c -r -I .
	
LD 					:= ld
LDFLAGS 			:= -m elf_i386 -x -s -T $(KERNEL_LINK_SCRIPT)

EMU					:= qemu-system-i386
EMUFLAGS			:= -monitor stdio -k en-gb -m 16M \
						-drive media=disk,format=raw,file=$(OUTPUT_IMAGE)

DDARGS				:= bs=512 conv=notrunc

.PHONY: all
all: clean $(OUTPUT_IMAGE)

boot/boot.bin: boot/boot.asm
	$(ASM) $< -o $@ $(ASMFLAGS) 

kernel/kmain.o: kernel/kmain.c
	$(CC) $< -o $@ $(CFLAGS) 

kernel/kernel.bin: kernel/kmain.o $(KERNEL_LINK_SCRIPT)
	$(LD) $< -o $@ $(LDFLAGS)

.PHONY: prepared_output_floppy
prepared_output_floppy:
	@echo "Preparing blank floppy of size $(OUTPUT_IMAGE_SIZE)"
	@mkdir -pv $(OUTPUT_DIR)
	@rm -fv $(OUTPUT_IMAGE)
	dd of=$(OUTPUT_IMAGE) if=/dev/zero bs=$(OUTPUT_IMAGE_SIZE) count=1

$(OUTPUT_IMAGE): boot/boot.bin kernel/kernel.bin prepared_output_floppy
	dd of=$@ if=boot/boot.bin seek=0 $(DDARGS)
	dd of=$@ if=kernel/kernel.bin seek=2 $(DDARGS)

boot.img: boot.bin
	@rm -fv $@
	dd if=/dev/zero of=$@ bs=1474560 count=1
	dd if=$< of=$@ bs=512 count=$(BOOT_SECTOR_COUNT) seek=0 conv=notrunc

.PHONY: clean
clean:
	@rm -frv **/*.img **/*.bin **/*.o $(OUTPUT_DIR)

.PHONY: run
run: $(OUTPUT_IMAGE)
	$(EMU) $(EMUFLAGS)

.PHONY: mount
mount: boot.img
	@if [ -d $(MOUNT_DIR) ]; then \
		echo "Already mounted. If this is wrong, try 'make unmount'."; \
	else \
		mkdir -v $(MOUNT_DIR); \
		sudo mount -v -t msdos -o loop $(OUTPUT_IMAGE) $(MOUNT_DIR); \
	fi

.PHONY: unmount
unmount:
	@if [ -d $(MOUNT_DIR) ]; then \
		sudo umount -v $(MOUNT_DIR); \
		rm -frv $(MOUNT_DIR); \
	else \
		echo "Not mounted"; \
	fi
