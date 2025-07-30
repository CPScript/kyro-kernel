# Test Makefile to verify bootloader
ASM = nasm
DD = dd
QEMU = qemu-system-i386

BUILD_DIR = build
BOOT_BIN = $(BUILD_DIR)/boot.bin
TEST_KERNEL_BIN = $(BUILD_DIR)/test_kernel.bin
OS_IMAGE = test-os-image.bin

.PHONY: test clean

test: $(OS_IMAGE)
	@echo "Starting test OS..."
	$(QEMU) -drive format=raw,file=$(OS_IMAGE),index=0,if=floppy -m 512M

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build bootloader
$(BOOT_BIN): boot/boot.asm | $(BUILD_DIR)
	@echo "Building bootloader..."
	$(ASM) -f bin $< -o $@

# Build test kernel
$(TEST_KERNEL_BIN): test_kernel.asm | $(BUILD_DIR)
	@echo "Building test kernel..."
	$(ASM) -f bin $< -o $@

# Create OS image
$(OS_IMAGE): $(BOOT_BIN) $(TEST_KERNEL_BIN)
	@echo "Creating test OS image..."
	$(DD) if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880 2>/dev/null
	$(DD) if=$(BOOT_BIN) of=$(OS_IMAGE) bs=512 count=1 conv=notrunc 2>/dev/null
	$(DD) if=$(TEST_KERNEL_BIN) of=$(OS_IMAGE) bs=512 seek=1 conv=notrunc 2>/dev/null
	@echo "Test OS image built: $(OS_IMAGE)"

clean:
	rm -f $(OS_IMAGE) $(BUILD_DIR)/*.bin