PREFIX ?= i686-elf
CROSS_BIN ?= $(HOME)/opt/cross/bin

CC = $(CROSS_BIN)/$(PREFIX)-gcc
LD = $(CROSS_BIN)/$(PREFIX)-ld
AS = $(CROSS_BIN)/$(PREFIX)-as

CFLAGS ?= -O2 -g

CFLAGS := $(CFLAGS) -std=gnu11 -ffreestanding -Wall -Wextra -Werror \
	-fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs
LDFLAGS := $(LDFLAGS) -T kernel/linker.ld
LIBS := $(LIBS)

BOOT_SRCS := boot/boot.s kernel/setGdt.s kernel/reloadSegments.s kernel/setIdt.s kernel/isr_asm.s
KERNEL_SRCS := kernel/kernel.c kernel/gdt.c kernel/idt.c kernel/isr.c kernel/pic.c kernel/keyboard.c

BOOT_OBJS := $(BOOT_SRCS:.s=.o)
KERNEL_OBJS := $(KERNEL_SRCS:.c=.o)
OBJS := $(BOOT_OBJS) $(KERNEL_OBJS)

TARGET := myos
ISO := myos.iso
ISODIR := isodir
GRUB_CFG_SRC ?= grub.cfg
MKRESCUE ?= grub2-mkrescue

.PHONY: all clean fclean re run
.SUFFIXES: .o .c .s

all: $(ISO)

%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $< -o $@

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

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
