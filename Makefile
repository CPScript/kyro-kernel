ASM = nasm
CC = gcc
LD = ld
DD = dd
QEMU = qemu-system-i386

BOOT_DIR = boot
KERNEL_DIR = kernel
INCLUDE_DIR = include
LIB_DIR = lib
TOOLS_DIR = tools
BUILD_DIR = build

# Output files
BOOT_BIN = $(BUILD_DIR)/boot.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
OS_IMAGE = os-image.bin

# Compiler flags for 32-bit cross-compilation
CFLAGS = -m32 -nostdlib -nostartfiles -nodefaultlibs -fno-builtin -fno-stack-protector \
         -nostdinc -fno-pie -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
         -I$(INCLUDE_DIR) -I$(KERNEL_DIR) -Wall -Wextra -g -O0

# Linker flags
LDFLAGS = -m elf_i386 -T $(KERNEL_DIR)/kernel.ld

# Assembly flags
ASMFLAGS = -f elf32 -g

# Bootloader assembly flags (binary output)
BOOT_ASMFLAGS = -f bin

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Kernel source files (exclude problematic files for now)
KERNEL_SOURCES = $(shell find $(KERNEL_DIR) -name "*.c" | grep -v printf_stubs.c)
KERNEL_ASM_SOURCES = $(shell find $(KERNEL_DIR) -name "*.s")

# Object files
KERNEL_OBJECTS = $(KERNEL_SOURCES:%.c=$(BUILD_DIR)/%.o) $(KERNEL_ASM_SOURCES:%.s=$(BUILD_DIR)/%.o)
KERNEL_OBJECTS += $(BUILD_DIR)/$(TOOLS_DIR)/init.o

# Library sources
LIB_SOURCES = $(shell find $(LIB_DIR) -name "*.c")
LIB_OBJECTS = $(LIB_SOURCES:%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean run debug info help test-boot

all: $(BUILD_DIR) $(OS_IMAGE)

# Build OS image with size checking
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	@echo "Creating OS image..."
	@KERNEL_SIZE=$$(stat -c%s $(KERNEL_BIN) 2>/dev/null || stat -f%z $(KERNEL_BIN) 2>/dev/null || echo 32768); \
	KERNEL_SECTORS=$$((($${KERNEL_SIZE} + 511) / 512)); \
	echo "Kernel size: $${KERNEL_SIZE} bytes ($${KERNEL_SECTORS} sectors)"; \
	if [ $${KERNEL_SECTORS} -gt 64 ]; then \
		echo "WARNING: Kernel is $${KERNEL_SECTORS} sectors, bootloader configured for 64"; \
		echo "You may need to increase KERNEL_SECTORS in boot.asm"; \
	fi
	$(DD) if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880 2>/dev/null
	$(DD) if=$(BOOT_BIN) of=$(OS_IMAGE) bs=512 count=1 conv=notrunc 2>/dev/null
	$(DD) if=$(KERNEL_BIN) of=$(OS_IMAGE) bs=512 seek=1 conv=notrunc 2>/dev/null
	@echo "OS image built: $(OS_IMAGE)"

# Build bootloader
$(BOOT_BIN): $(BOOT_DIR)/boot.asm | $(BUILD_DIR)
	@echo "Building bootloader..."
	$(ASM) $(BOOT_ASMFLAGS) $< -o $@
	@BOOT_SIZE=$$(stat -c%s $@ 2>/dev/null || stat -f%z $@ 2>/dev/null); \
	if [ $${BOOT_SIZE} -ne 512 ]; then \
		echo "ERROR: Bootloader size is $${BOOT_SIZE} bytes, must be exactly 512"; \
		exit 1; \
	fi
	@echo "Bootloader size check passed (512 bytes)"

# Build kernel
$(KERNEL_BIN): $(KERNEL_OBJECTS) $(LIB_OBJECTS) $(KERNEL_DIR)/kernel.ld | $(BUILD_DIR)
	@echo "Linking kernel..."
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJECTS) $(LIB_OBJECTS)
	@echo "Kernel linked successfully"

# Compile C source files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly files
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "Assembling $<..."
	$(ASM) $(ASMFLAGS) $< -o $@

# Run in QEMU
run: $(OS_IMAGE)
	@echo "Starting Kyro OS in QEMU..."
	$(QEMU) -drive format=raw,file=$(OS_IMAGE),index=0,if=floppy -m 512M

# Debug in QEMU
debug: $(OS_IMAGE)
	@echo "Starting Kyro OS in QEMU with debugging..."
	$(QEMU) -drive format=raw,file=$(OS_IMAGE),index=0,if=floppy -m 512M -s -S -d int,cpu_reset

# Test bootloader only
test-boot: $(BOOT_BIN)
	@echo "Testing bootloader with simple kernel..."
	$(ASM) -f bin test_kernel.asm -o $(BUILD_DIR)/test_kernel.bin
	$(DD) if=/dev/zero of=test-os-image.bin bs=512 count=2880 2>/dev/null
	$(DD) if=$(BOOT_BIN) of=test-os-image.bin bs=512 count=1 conv=notrunc 2>/dev/null
	$(DD) if=$(BUILD_DIR)/test_kernel.bin of=test-os-image.bin bs=512 seek=1 conv=notrunc 2>/dev/null
	$(QEMU) -drive format=raw,file=test-os-image.bin,index=0,if=floppy -m 512M

# Clean build files
clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR) $(OS_IMAGE) test-os-image.bin

# Show build information
info:
	@echo "Build System"
	@echo "==================="
	@echo "Bootloader: $(BOOT_BIN)"
	@echo "Kernel:     $(KERNEL_BIN)"
	@echo "OS Image:   $(OS_IMAGE)"
	@echo ""
	@echo "Kernel sources: $(words $(KERNEL_SOURCES)) files"
	@echo "Library sources: $(words $(LIB_SOURCES)) files"
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build complete OS image"
	@echo "  run       - Build and run in QEMU"
	@echo "  debug     - Build and run in QEMU with debugging"
	@echo "  test-boot - Test bootloader with simple kernel"
	@echo "  clean     - Remove all build files"
	@echo "  info      - Show this information"

help: info

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing build dependencies..."
	sudo apt update
	sudo apt install gcc-multilib nasm qemu-system-x86 build-essential

# Check if required tools are available
check-tools:
	@echo "Checking build tools..."
	@which $(CC) > /dev/null || (echo "ERROR: $(CC) not found" && exit 1)
	@which $(ASM) > /dev/null || (echo "ERROR: $(ASM) not found" && exit 1)
	@which $(LD) > /dev/null || (echo "ERROR: $(LD) not found" && exit 1)
	@which $(QEMU) > /dev/null || (echo "ERROR: $(QEMU) not found" && exit 1)
	@echo "All tools available!"
