# Global Makefile
BUILD_DIR=build
BOOTLOADER_SRC=bootloader/bootloader.asm
BOOTLOADER_BIN=$(BUILD_DIR)/bootloader/bootloader.o
OS=$(BUILD_DIR)/os/kernel.bin
DISK_IMG=disk.img

all: bootdisk

.PHONY: bootdisk bootloader os

os:
	make -C os

bootloader: os
	mkdir -p $(dir $(BOOTLOADER_BIN))
	$(eval KERNEL_SIZE := $(shell stat -c%s $(OS)))
	$(eval SECTORS := $(shell echo $$(( ($(KERNEL_SIZE) + 511) / 512 )) ))
	@echo "Kernel size: $(KERNEL_SIZE) bytes ($(SECTORS) sectors)"
	nasm -f bin $(BOOTLOADER_SRC) -DSECTORS_TO_READ=$(SECTORS) -o $(BOOTLOADER_BIN)

bootdisk: bootloader os
	dd if=/dev/zero of=$(DISK_IMG) bs=512 count=2880
	dd conv=notrunc if=$(BOOTLOADER_BIN) of=$(DISK_IMG) bs=512 count=1 seek=0
	dd conv=notrunc if=$(OS) of=$(DISK_IMG) bs=512 seek=1

qemu:
	qemu-system-i386 -machine q35 -fda $(DISK_IMG) -gdb tcp::26000 -S

clean:
	make -C bootloader clean
	make -C os clean
	rm -f $(DISK_IMG)