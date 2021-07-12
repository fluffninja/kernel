## Output Options ##
OUTPUT_DIR := out
OUTPUT_IMAGE := $(OUTPUT_DIR)/bootdisk.img
OUTPUT_IMAGE_SIZE := 1474560
DIRTY_FILE_EXTENSIONS := a o bin elf map

## Assembler Options ##
AS := nasm

# Emulator Options
EMULATOR := qemu-system-i386
EMULATOR_FLAGS := -monitor stdio
EMULATOR_FLAGS += -k en-us
EMULATOR_FLAGS += -m 16M
EMULATOR_FLAGS += -drive media=disk,format=raw,file=$(OUTPUT_IMAGE)

## Recipes ##

.PHONY: all
all: image


.PHONY: bootloader
bootloader: boot/boot.bin


boot/boot.bin: boot/boot.asm
	@echo "\n### bootloader ###"
	$(AS) -f bin $< -o $@ 


.PHONY: libc
libc:
	@echo "\n### libc ###"
	$(MAKE) -C libc/ libc.a


.PHONY: kernel
kernel: libc
	@echo "\n### kernel ###"
	$(MAKE) -C kernel/ kernel.bin


.PHONY: image
image: $(OUTPUT_IMAGE)


$(OUTPUT_IMAGE): boot/boot.bin kernel
	@echo "\n### output image ###"
	@mkdir -pv $(OUTPUT_DIR)
	@echo "Preparing blank floppy of size $(OUTPUT_IMAGE_SIZE)"
	@rm -fv $(OUTPUT_IMAGE)
	dd of=$(OUTPUT_IMAGE) if=/dev/zero bs=$(OUTPUT_IMAGE_SIZE) count=1
	dd of=$@ if=boot/boot.bin seek=0 bs=512 conv=notrunc status=noxfer
	dd of=$@ if=kernel/kernel.bin seek=2 bs=512 conv=notrunc status=noxfer


.PHONY: clean
clean:
	@rm -frv $(OUTPUT_DIR)
	@echo "Cleaning..."
	@for extension in $(DIRTY_FILE_EXTENSIONS); do \
		find . -type f -name "*.$$extension" -print -delete; \
	done
	@echo "Done."


.PHONY: run
run: image
	$(EMULATOR) $(EMULATOR_FLAGS)
