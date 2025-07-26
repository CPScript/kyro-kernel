CC = gcc
LD = ld
NASM = nasm

CFLAGS = -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32 -Iinclude
LDFLAGS = -Ttext 0x1000 --oformat binary -m elf_i386

KERNEL_DIR = kernel
DRIVERS_DIR = kernel/drivers
UTILS_DIR = kernel/utils
INTERRUPTS_DIR = kernel/interrupts
MEMORY_DIR = kernel/memory
LIB_DIR = lib
TOOLS_DIR = tools

# Object files
KERNEL_OBJS = kernel.o fs.o terminal.o user.o shell.o networking.o \
              package_manager.o scripting.o run_shell.o run_terminal.o login.o

DRIVER_OBJS = keyboard.o mouse.o network.o

UTIL_OBJS = memory.o string.o math.o

SYSTEM_OBJS = idt.o paging.o timer.o syscall.o process.o io.o

STDIO_OBJS = stdio.o

INIT_OBJS = init.o

all: build-os-image

build-os-image: build-boot-loader build-kernel
	cat boot.bin kernel.bin > os-image.bin

build-boot-loader: boot/boot.asm
	$(NASM) -f bin boot/boot.asm -o boot.bin

build-kernel: $(KERNEL_OBJS) $(DRIVER_OBJS) $(UTIL_OBJS) $(SYSTEM_OBJS) $(STDIO_OBJS) $(INIT_OBJS)
	$(LD) $(LDFLAGS) -o kernel.bin $^

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

# System objects (new)
idt.o: $(INTERRUPTS_DIR)/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

paging.o: $(MEMORY_DIR)/paging.c
	$(CC) $(CFLAGS) -c $< -o $@

timer.o: $(KERNEL_DIR)/timer.c
	$(CC) $(CFLAGS) -c $< -o $@

syscall.o: $(KERNEL_DIR)/syscall.c
	$(CC) $(CFLAGS) -c $< -o $@

process.o: $(KERNEL_DIR)/process.c
	$(CC) $(CFLAGS) -c $< -o $@

io.o: $(KERNEL_DIR)/io.c
	$(CC) $(CFLAGS) -c $< -o $@

# Library objects
stdio.o: $(LIB_DIR)/stdio/stdio.c
	$(CC) $(CFLAGS) -c $< -o $@

# Tools objects
init.o: $(TOOLS_DIR)/init.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.bin *.o

.PHONY: all build-os-image build-boot-loader build-kernel clean
