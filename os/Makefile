# OS Makefile
BUILD_DIR=../build/os
TARGET=$(BUILD_DIR)/kernel.bin
ELF_TARGET=$(BUILD_DIR)/kernel.elf

ASM_SRCS := $(shell find src -type f -name '*.asm')
C_SRCS   := $(shell find src -type f -name '*.c')

ASM_OBJS := $(patsubst src/%.asm, $(BUILD_DIR)/%.o, $(ASM_SRCS))
C_OBJS   := $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(C_SRCS))

CFLAGS := -m32 -ffreestanding -fno-pic -g -O0 -Iinclude

all: $(TARGET)

$(TARGET): $(ELF_TARGET)
	ld -m elf_i386 -T linker.ld $(ASM_OBJS) $(C_OBJS) -o $@ --oformat binary

$(ELF_TARGET): $(ASM_OBJS) $(C_OBJS) linker.ld
	mkdir -p $(dir $@)
	ld -m elf_i386 -T linker.ld $(ASM_OBJS) $(C_OBJS) -o $@

$(BUILD_DIR)/%.o: src/%.asm
	mkdir -p $(dir $@)
	nasm -f elf32 -g $< -o $@

$(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean