CC = gcc
LD = ld
NASM = nasm

# Fixed CFLAGS with proper include paths
CFLAGS = -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32 \
         -I. -Iinclude -Ikernel -Ikernel/drivers -Ikernel/interrupts -Ikernel/memory \
         -Ikernel/utils -Ilib/stdio -Iinclude/fs -fno-pie -fno-pic -Wall -Wextra \
         -DDEBUG -g

LDFLAGS = -Ttext 0x1000 --oformat binary -m elf_i386 -nostdlib

# Source dirs
KERNEL_DIR = kernel
DRIVERS_DIR = kernel/drivers
UTILS_DIR = kernel/utils
INTERRUPTS_DIR = kernel/interrupts
MEMORY_DIR = kernel/memory
LIB_DIR = lib
TOOLS_DIR = tools

# Object files - organized
KERNEL_OBJS = kernel.o fs.o terminal.o user.o shell.o networking.o \
              package_manager.o scripting.o run_shell.o run_terminal.o \
              login.o printf.o timer.o syscall.o process.o io.o pic.o

DRIVER_OBJS = keyboard.o mouse.o network.o

UTIL_OBJS = memory.o string.o math.o

INTERRUPT_OBJS = idt.o interrupt_asm.o

MEMORY_OBJS = paging.o

STDIO_OBJS = stdio.o

INIT_OBJS = init.o

# Object files
ALL_OBJS = $(KERNEL_OBJS) $(DRIVER_OBJS) $(UTIL_OBJS) $(INTERRUPT_OBJS) \
           $(MEMORY_OBJS) $(STDIO_OBJS) $(INIT_OBJS)

.PHONY: all build-os-image build-boot-loader build-kernel clean clean-all debug size check run help

all: build-os-image

build-os-image: boot.bin kernel.bin
	dd if=/dev/zero of=os-image.bin bs=512 count=2880 2>/dev/null
	dd if=boot.bin of=os-image.bin bs=512 count=1 conv=notrunc 2>/dev/null
	dd if=kernel.bin of=os-image.bin bs=512 seek=1 conv=notrunc 2>/dev/null
	@echo "OS image built successfully: os-image.bin"

build-boot-loader: boot.bin

boot.bin: boot/boot.asm
	$(NASM) -f bin boot/boot.asm -o boot.bin
	@echo "Bootloader built: boot.bin"

build-kernel: kernel.bin

kernel.bin: $(ALL_OBJS)
	$(LD) $(LDFLAGS) -o kernel.bin $^
	@echo "Kernel built: kernel.bin"

# Kernel objects
kernel.o: $(KERNEL_DIR)/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

fs.o: $(KERNEL_DIR)/fs.c
	$(CC) $(CFLAGS) -c $< -o $@

terminal.o: $(KERNEL_DIR)/terminal.c
	$(CC) $(CFLAGS) -c $< -o $@

user.o: $(KERNEL_DIR)/user.c
	$(CC) $(CFLAGS) -c $< -o $@

shell.o: $(KERNEL_DIR)/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

networking.o: $(KERNEL_DIR)/networking.c
	$(CC) $(CFLAGS) -c $< -o $@

package_manager.o: $(KERNEL_DIR)/package_manager.c
	$(CC) $(CFLAGS) -c $< -o $@

scripting.o: $(KERNEL_DIR)/scripting.c
	$(CC) $(CFLAGS) -c $< -o $@

run_shell.o: $(KERNEL_DIR)/run_shell.c
	$(CC) $(CFLAGS) -c $< -o $@

run_terminal.o: $(KERNEL_DIR)/run_terminal.c
	$(CC) $(CFLAGS) -c $< -o $@

login.o: $(KERNEL_DIR)/login.c
	$(CC) $(CFLAGS) -c $< -o $@

printf.o: $(KERNEL_DIR)/printf.c
	$(CC) $(CFLAGS) -c $< -o $@

timer.o: $(KERNEL_DIR)/timer.c
	$(CC) $(CFLAGS) -c $< -o $@

syscall.o: $(KERNEL_DIR)/syscall.c
	$(CC) $(CFLAGS) -c $< -o $@

process.o: $(KERNEL_DIR)/process.c
	$(CC) $(CFLAGS) -c $< -o $@

io.o: $(KERNEL_DIR)/io.c
	$(CC) $(CFLAGS) -c $< -o $@

pic.o: $(KERNEL_DIR)/pic.c
	$(CC) $(CFLAGS) -c $< -o $@

# Driver objects
keyboard.o: $(DRIVERS_DIR)/keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@

mouse.o: $(DRIVERS_DIR)/mouse.c
	$(CC) $(CFLAGS) -c $< -o $@

network.o: $(DRIVERS_DIR)/network.c
	$(CC) $(CFLAGS) -c $< -o $@

# Utility objects
memory.o: $(UTILS_DIR)/memory.c
	$(CC) $(CFLAGS) -c $< -o $@

string.o: $(UTILS_DIR)/string.c
	$(CC) $(CFLAGS) -c $< -o $@

math.o: $(UTILS_DIR)/math.c
	$(CC) $(CFLAGS) -c $< -o $@

# Interrupt objects
idt.o: $(INTERRUPTS_DIR)/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

interrupt_asm.o: $(INTERRUPTS_DIR)/interrupt_asm.s
	$(NASM) -f elf32 $< -o $@

# Memory objects
paging.o: $(MEMORY_DIR)/paging.c
	$(CC) $(CFLAGS) -c $< -o $@

# Library objects
stdio.o: $(LIB_DIR)/stdio/stdio.c
	$(CC) $(CFLAGS) -c $< -o $@

# Tools objects
init.o: $(TOOLS_DIR)/init.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean targets
clean:
	rm -f *.bin *.o
	@echo "Cleaned build files"

clean-all: clean
	rm -f os-image.bin
	@echo "Cleaned all generated files"

# Force rebuild everything
rebuild: clean all

# Debug targets
debug: CFLAGS += -g -DDEBUG
debug: all

# Size information
size: build-kernel build-boot-loader
	@echo "Build size information:"
	@echo "Boot sector size:"
	@ls -lh boot.bin 2>/dev/null || echo "boot.bin not found"
	@echo "Kernel size:"
	@ls -lh kernel.bin 2>/dev/null || echo "kernel.bin not found"
	@echo "OS image size:"
	@ls -lh os-image.bin 2>/dev/null || echo "os-image.bin not found"

# Check for common issues
check:
	@echo "Checking for common build issues..."
	@echo "Checking if all source files exist..."
	@test -f boot/boot.asm || echo "ERROR: boot/boot.asm not found"
	@test -f $(KERNEL_DIR)/kernel.c || echo "ERROR: kernel/kernel.c not found"
	@test -f $(DRIVERS_DIR)/keyboard.c || echo "ERROR: keyboard driver not found"
	@test -f $(DRIVERS_DIR)/mouse.c || echo "ERROR: mouse driver not found"
	@test -f $(DRIVERS_DIR)/network.c || echo "ERROR: network driver not found"
	@echo "Check complete"

# minimal bootloader (debugging)
test-minimal: boot/boot.asm
	@echo "Building minimal test bootloader..."
	@echo '[BITS 16]' > test_boot.asm
	@echo '[ORG 0x7C00]' >> test_boot.asm
	@echo 'start:' >> test_boot.asm
	@echo '    mov ax, 0x0000' >> test_boot.asm
	@echo '    mov ds, ax' >> test_boot.asm
	@echo '    mov si, msg' >> test_boot.asm
	@echo '    mov ah, 0x0E' >> test_boot.asm
	@echo 'loop: lodsb' >> test_boot.asm
	@echo '    cmp al, 0' >> test_boot.asm
	@echo '    je hang' >> test_boot.asm
	@echo '    int 0x10' >> test_boot.asm
	@echo '    jmp loop' >> test_boot.asm
	@echo 'hang: hlt' >> test_boot.asm
	@echo '    jmp hang' >> test_boot.asm
	@echo 'msg db "TEST BOOTLOADER OK", 0' >> test_boot.asm
	@echo 'times 510-($$-$$) db 0' >> test_boot.asm
	@echo 'dw 0xAA55' >> test_boot.asm
	$(NASM) -f bin test_boot.asm -o test_boot.bin
	@echo "Test bootloader created: test_boot.bin"
	@echo "Run with: qemu-system-i386 -drive format=raw,file=test_boot.bin"

# Run in QEMU
run: build-os-image
	@if command -v qemu-system-i386 >/dev/null 2>&1; then \
		echo "Running OS in QEMU..."; \
		qemu-system-i386 -drive format=raw,file=os-image.bin; \
	else \
		echo "QEMU not found. Install qemu-system-i386 to run the OS."; \
	fi

# Run with debugging
run-debug: build-os-image
	@if command -v qemu-system-i386 >/dev/null 2>&1; then \
		echo "Running OS in QEMU with debugging..."; \
		qemu-system-i386 -drive format=raw,file=os-image.bin -monitor stdio; \
	else \
		echo "QEMU not found. Install qemu-system-i386 to run the OS."; \
	fi

help:
	@echo "Available targets:"
	@echo "  all (default)      - Build complete OS image"
	@echo "  build-os-image     - Build OS image (bootloader + kernel)"
	@echo "  build-os-image-simple - Build OS image using simple cat method"
	@echo "  build-boot-loader  - Build bootloader only"
	@echo "  build-kernel       - Build kernel only"
	@echo "  clean              - Remove object files and binaries"
	@echo "  clean-all          - Remove all generated files"
	@echo "  rebuild            - Clean and rebuild everything"
	@echo "  debug              - Build with debug symbols"
	@echo "  size               - Show size information"
	@echo "  check              - Check for missing source files"
	@echo "  test-minimal       - Create minimal test bootloader"
	@echo "  run                - Run OS in QEMU (if available)"
	@echo "  run-debug          - Run OS in QEMU with monitor"
	@echo "  help               - Show this help message"
