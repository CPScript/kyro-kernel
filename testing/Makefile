ASM = nasm
DD = dd
QEMU = qemu-system-i386

BOOT_DIR = boot

BOOTLOADER = $(BOOT_DIR)/boot.bin
KERNEL = $(BOOT_DIR)/kernel.bin
OS_IMAGE = os-image.bin

ASM_FLAGS = -f bin
QEMU_FLAGS = -drive format=raw,file=$(OS_IMAGE),index=0,if=floppy -m 512M

.PHONY: all clean run debug

all: $(OS_IMAGE)

$(OS_IMAGE): $(BOOTLOADER) $(KERNEL)
	@echo "Building OS image..."
	$(DD) if=$(BOOTLOADER) of=$(OS_IMAGE) bs=512 count=1 conv=notrunc
	$(DD) if=$(KERNEL) of=$(OS_IMAGE) bs=512 seek=1 conv=notrunc
	@echo "OS image built successfully: $(OS_IMAGE)"

$(BOOTLOADER): $(BOOT_DIR)/boot.asm
	@echo "Building bootloader..."
	$(ASM) $(ASM_FLAGS) $(BOOT_DIR)/boot.asm -o $(BOOTLOADER)

$(KERNEL): $(BOOT_DIR)/kernel.asm
	@echo "Building kernel..."
	$(ASM) $(ASM_FLAGS) $(BOOT_DIR)/kernel.asm -o $(KERNEL)

run: $(OS_IMAGE)
	@echo "Starting Kyro OS in QEMU..."
	$(QEMU) $(QEMU_FLAGS)

debug: $(OS_IMAGE)
	@echo "Starting Kyro OS in QEMU with debugging..."
	$(QEMU) $(QEMU_FLAGS) -s -S -monitor stdio

clean:
	@echo "Cleaning build files..."
	rm -f $(BOOTLOADER) $(KERNEL) $(OS_IMAGE)
	@echo "Clean complete."

info:
	@echo "Kyro OS Build System"
	@echo "==================="
	@echo "Bootloader: $(BOOTLOADER)"
	@echo "Kernel:     $(KERNEL)"
	@echo "OS Image:   $(OS_IMAGE)"
	@echo ""
	@echo "Available targets:"
	@echo "  all     - Build complete OS image"
	@echo "  run     - Build and run in QEMU"
	@echo "  debug   - Build and run in QEMU with debugging"
	@echo "  clean   - Remove all build files"
	@echo "  info    - Show this information"

help: info
