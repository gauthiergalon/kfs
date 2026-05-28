PREFIX ?= i686-elf
# Directory where a local cross toolchain was installed, e.g. /home/USER/opt/cross/bin
# You can override this when calling make: `make CROSS_BIN=/path/to/cross/bin`
CROSS_BIN ?= $(HOME)/opt/cross/bin

# If the tool exists under CROSS_BIN use the full path, otherwise fall back to
# using the prefixed tools available in PATH (e.g. i686-elf-gcc)
ifeq ($(wildcard $(CROSS_BIN)/$(PREFIX)-gcc),)
CC = $(PREFIX)-gcc
LD = $(PREFIX)-ld
OBJCOPY = $(PREFIX)-objcopy
else
CC = $(CROSS_BIN)/$(PREFIX)-gcc
LD = $(CROSS_BIN)/$(PREFIX)-ld
OBJCOPY = $(CROSS_BIN)/$(PREFIX)-objcopy
endif

# Build flags per project spec
CFLAGS := -std=gnu11 -ffreestanding -O2 -Wall -Wextra \
	-fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs
ASFLAGS := -m32
LDFLAGS := -T kernel/linker.ld

SRCS := kernel/kernel.c
OBJS := boot/boot.o kernel/kernel.o

TARGET := myos
ISO := myos.iso
ISODIR := isodir

# grub mkrescue command (Fedora often provides grub2-mkrescue)
MKRESCUE ?= grub2-mkrescue

# Source grub config in the repo (default `grub.cfg` at project root)
GRUB_CFG_SRC ?= grub.cfg

.PHONY: all clean fclean re run

all: $(ISO)

## assembly
boot/boot.o: boot/boot.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

## C compilation
kernel/kernel.o: kernel/kernel.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

## Link kernel
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(TARGET)

## Prepare isodir and copy kernel
$(ISODIR)/boot/$(TARGET): $(TARGET)
	@mkdir -p $(dir $@)
	cp $< $@

## Ensure grub.cfg is present inside the iso directory
$(ISODIR)/boot/grub/grub.cfg: $(GRUB_CFG_SRC)
	@mkdir -p $(dir $@)
	cp $< $@

## Build ISO
$(ISO): $(ISODIR)/boot/$(TARGET) $(ISODIR)/boot/grub/grub.cfg
	$(MKRESCUE) -o $(ISO) $(ISODIR)

## Run with QEMU (requires myos.iso)
run: $(ISO)
	qemu-system-i386 -enable-kvm -m 512 -smp 2 -cdrom $(ISO) -boot d -display gtk

## Clean intermediates (keep grub.cfg copy)
clean:
	rm -f $(OBJS)
	rm -f $(ISODIR)/boot/$(TARGET)
	# remove empty dirs if present
	rmdir -p $(ISODIR)/boot 2>/dev/null || true

## Full clean (remove generated files incl. iso and kernel)
fclean: clean
	rm -f $(TARGET) $(ISO)
	rm -rf $(ISODIR)

## Rebuild from scratch
re: fclean all

