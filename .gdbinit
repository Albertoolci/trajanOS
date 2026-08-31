# Architecture and disassembly flavor settings for GDB
set architecture i8086
set disassembly-flavor intel

# Target connection settings for GDB
target remote localhost:26000

# Load kernel symbols from the specified ELF file
define load-kernel
    symbol-file build/os/kernel.elf
    echo \n[GDB] Kernel symbols loaded from build/os/kernel.elf\n
end

load-kernel

# Set a breakpoint at the entry point of the bootloader (0x7c00)
b *0x7c00

# Hook to display the current instruction and its address when the program stops
define hook-stop
    if ($cs == 0x8 || $cs == 0x10 || $cs == 0x08)
        printf "[PM 32-bit @ 0x%x] ", $eip
        x/i $eip
    else
        printf "[RM 16-bit @ %04x:%04x (Phys: 0x%x)] ", $cs, $eip, ($cs*16 + $eip)
        x/i ($cs*16 + $eip)
    end
end

# Visual layout of registers and dissasembly
layout asm
layout reg