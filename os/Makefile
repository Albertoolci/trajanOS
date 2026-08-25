# OS Makefile
BUILD_DIR=../build/os
TARGET=$(BUILD_DIR)/kernel.bin

ASM_SRCS := $(shell find src -type f -name '*.asm')
C_SRCS   := $(shell find src -type f -name '*.c')

ASM_OBJS := $(patsubst src/%.asm, $(BUILD_DIR)/%.o, $(ASM_SRCS))
C_OBJS   := $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(C_SRCS))

CFLAGS := -m32 -ffreestanding -fno-pic -Iinclude

all: $(TARGET)

$(TARGET): $(ASM_OBJS) $(C_OBJS) linker.ld
	mkdir -p $(dir $@)
	ld -m elf_i386 -T linker.ld $(ASM_OBJS) $(C_OBJS) -o $@ --oformat binary

$(BUILD_DIR)/%.o: src/%.asm
	mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean