OUTPUT_DIR		:= out
OUTPUT_IMAGE		:= $(OUTPUT_DIR)/bootdisk.img
OUTPUT_IMAGE_SIZE	:= 1474560
DIRTY_FILE_EXTENSIONS	:= a o bin elf map
MOUNT_DIR		:= $(OUTPUT_DIR)/bootdisk_mout/
ASM			:= nasm
EMULATOR		:= qemu-system-i386
EMULATOR_FLAGS		:= -monitor stdio -k en-gb -m 16M \
			-drive media=disk,format=raw,file=$(OUTPUT_IMAGE)
DDFLAGS			:= bs=512 conv=notrunc status=noxfer

.PHONY: all
all: image

# The bootloader
boot/boot.bin: boot/boot.asm
	$(ASM) $< -o $@ -f bin

# libc (see libc/Makefile)
.PHONY: libc
libc:
	$(MAKE) -C libc/ libc.a

# The kernel itself (see kernel/Makefile)
.PHONY: kernel
kernel: libc
	$(MAKE) -C kernel/ kernel.bin

# Shortcut to create the output image (see below)
.PHONY: image
image: $(OUTPUT_IMAGE)

# Create the output image (most likely a floppy).
# Involves creating the blank medium (see below), and then writing the binary
# output onto it.
$(OUTPUT_IMAGE): boot/boot.bin kernel image_medium
	dd of=$@ if=boot/boot.bin seek=0 $(DDFLAGS)
	dd of=$@ if=kernel/kernel.bin seek=2 $(DDFLAGS)

# Create the output image medium
.PHONY: image_medium
image_medium:
	@echo "Preparing blank floppy of size $(OUTPUT_IMAGE_SIZE)"
	@mkdir -pv $(OUTPUT_DIR)
	@rm -fv $(OUTPUT_IMAGE)
	dd of=$(OUTPUT_IMAGE) if=/dev/zero bs=$(OUTPUT_IMAGE_SIZE) count=1

# Deletes dirty files - anythin that's output by the build-process.
# Specified by extension in DIRTY_FILE_EXTENSIONS.
# Also deletes the output image.
.PHONY: clean
clean:
	@rm -frv $(OUTPUT_DIR)
	@echo "Deleting dirty files:"
	@for extension in $(DIRTY_FILE_EXTENSIONS); do \
		find . -type f -name "*.$$extension" -print -delete; \
	done
	@echo "Done"

# Use the program specified by EMULATOR to emulate the kernel.
# By default: qemu-system-i386.
.PHONY: run
run:
	$(EMULATOR) $(EMULATOR_FLAGS)

# Attempt to mount the output image as a filesystem on the host.
# If the successful, the filesystem will be mounted at MOUNT_DIR - by default:
# out/bootdisk_mount/
.PHONY: mount
mount:
	@if [ -d $(MOUNT_DIR) ]; then \
		echo "Already mounted. If this is wrong try 'make unmount'"; \
	else \
		mkdir -v $(MOUNT_DIR); \
		sudo mount -v -t msdos -o loop $(OUTPUT_IMAGE) $(MOUNT_DIR); \
		echo "Mounted at $(MOUNT_DIR)"; \
	fi

.PHONY: unmount
unmount:
	@if [ -d $(MOUNT_DIR) ]; then \
		sudo umount -v $(MOUNT_DIR); \
		rm -frv $(MOUNT_DIR); \
		echo "Unmounted from $(MOUNT_DIR)"; \
	else \
		echo "Not mounted"; \
	fi
