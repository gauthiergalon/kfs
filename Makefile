PREFIX ?= i686-elf
CROSS_BIN ?= $(HOME)/opt/cross/bin

CC = $(CROSS_BIN)/$(PREFIX)-gcc
LD = $(CROSS_BIN)/$(PREFIX)-ld
AS = $(CROSS_BIN)/$(PREFIX)-as
OBJCOPY = $(CROSS_BIN)/$(PREFIX)-objcopy

CFLAGS := -std=gnu11 -ffreestanding -O2 -Wall -Wextra -Werror \
	-fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs
LDFLAGS := -T kernel/linker.ld

SRCS := kernel/kernel.c
OBJS := boot/boot.o kernel/kernel.o

TARGET := myos
ISO := myos.iso
ISODIR := isodir

MKRESCUE ?= grub2-mkrescue

GRUB_CFG_SRC ?= grub.cfg

.PHONY: all clean fclean re run

all: $(ISO)

boot/boot.o: boot/boot.s
	@mkdir -p $(dir $@)
	$(AS) $< -o $@

kernel/kernel.o: kernel/kernel.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(TARGET)

$(ISODIR)/boot/$(TARGET): $(TARGET)
	@mkdir -p $(dir $@)
	cp $< $@

$(ISODIR)/boot/grub/grub.cfg: $(GRUB_CFG_SRC)
	@mkdir -p $(dir $@)
	cp $< $@

$(ISO): $(ISODIR)/boot/$(TARGET) $(ISODIR)/boot/grub/grub.cfg
	$(MKRESCUE) -o $(ISO) $(ISODIR)

run: $(ISO)
	qemu-system-i386 -enable-kvm -m 512 -smp 2 -cdrom $(ISO) -boot d -display gtk

clean:
	rm -f $(OBJS)
	rm -f $(ISODIR)/boot/$(TARGET)
	rmdir -p $(ISODIR)/boot 2>/dev/null || true

fclean: clean
	rm -f $(TARGET) $(ISO)
	rm -rf $(ISODIR)

re: fclean all

