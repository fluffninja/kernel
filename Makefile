OUTPUT_DIR		:= out
OUTPUT_IMAGE		:= $(OUTPUT_DIR)/bootdisk.img
OUTPUT_IMAGE_SIZE	:= 1474560

MOUNT_DIR		:= $(OUTPUT_DIR)/bootdisk_mout/

KERNEL_LINK_SCRIPT	:= kernel/kernel.ld

ASM			:= nasm

EMU			:= qemu-system-i386
EMUFLAGS		:= -monitor stdio -k en-gb -m 16M \
			-drive media=disk,format=raw,file=$(OUTPUT_IMAGE)

DDFLAGS			:= bs=512 conv=notrunc

.PHONY: all
all: clean $(OUTPUT_IMAGE)

boot/boot.bin: boot/boot.asm
	$(ASM) $< -o $@ -f bin

kernel/kernel.bin: kernel/
	$(MAKE) -C kernel

.PHONY: prepared_output_floppy
prepared_output_floppy:
	@echo "Preparing blank floppy of size $(OUTPUT_IMAGE_SIZE)"
	@mkdir -pv $(OUTPUT_DIR)
	@rm -fv $(OUTPUT_IMAGE)
	dd of=$(OUTPUT_IMAGE) if=/dev/zero bs=$(OUTPUT_IMAGE_SIZE) count=1

$(OUTPUT_IMAGE): boot/boot.bin kernel/kernel.bin prepared_output_floppy
	dd of=$@ if=boot/boot.bin seek=0 $(DDFLAGS)
	dd of=$@ if=kernel/kernel.bin seek=2 $(DDFLAGS)

.PHONY: clean
clean:
	@rm -frv **/*.img **/*.bin **/*.o $(OUTPUT_DIR)

.PHONY: run
run: $(OUTPUT_IMAGE)
	$(EMU) $(EMUFLAGS)

.PHONY: mount
mount: $(OUTPUT_IMAGE)
	@if [ -d $(MOUNT_DIR) ]; then \
		echo "Already mounted. If this is wrong, try 'make unmount'"; \
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
