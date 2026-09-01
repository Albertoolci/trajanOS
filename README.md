# trajanOS

A 32-bit x86 bare-metal kernel written in C and NASM assembly, built without `libc` dependencies, targeting the 32-bit x86 (IA-32) architecture and Intel 80386 (i386) processor designed and implemented entirely by me with learning purpouses.

> **Note:** You can easily test and run trajanOS in a virtualized environment using **QEMU**, there is no need to do an installation on bare metal. Simply execute:
> ```bash
> make
> make run
> ```

## Current Progress

* **x86 Protected Mode:** 32-bit execution environment setup.
* **VGA Text Mode Driver (`0xB8000`):** Hardware cursor tracking, custom `kprintf` formatting, and automatic line scrolling.
* **IDT & Exception Handling:** Interrupt Descriptor Table configured with Assembly stubs (still to be written) for all 32 CPU exceptions.
* **8259 PIC Remapping:** Hardware IRQs mapped to vectors 32–47 to avoid overlap with CPU exceptions.
* **PIT Timer (IRQ 0):** Programmable Interval Timer configured at 100 Hz driving system ticks.
* **PS/2 Keyboard (IRQ 1):** Scancode handling via I/O port `0x60` with ASCII mapping.
* **Interactive CLI Shell:** Minimal Command Line Interface supporting built-in commands (`help`, `clear`, `version`, `ticks`).
* **Custom libc Subset:** Independent implementations of memory and string operations (`memcpy`, `strlen`, `strcmp`, `itoa`, `hex_to_ascii`).
* **Physical Memory Manager (`pmm`):** Bitmap-based Physical Memory Manager with 4KB page frames and BIOS E820 mapping

## Requirements

* `gcc` (with 32-bit build support `-m32`)
* `nasm`
* `ld` (GNU Linker)
* `qemu-system-x86`

On Debian/Ubuntu-based systems:
```bash
sudo apt install build-essential gcc-multilib nasm qemu-system-x86
```

## Build & Run
```bash
make
make run
```

### Test with `gdb`
```bash
make
make gdb

# On another terminal
gdb # .gdbinit already given with some configurations
```

### Clean build artifacts
```bash
make clean
```

## Repository Structure
### Kernel simplified structure
```
.
├── os/
│   ├── include/    # Headers (.h) organized by cpu/, drivers/, and libc/
│   ├── src/        # Implementation files (.c) and assembly stubs (.asm)
│   └── linker.ld   # Linker script defining kernel memory layout
└── build/          # Compiled object files (.o) and final kernel binary
```

### Directory structure
```
.
├── bootloader
│
└── os
    ├── include
    │   ├── cpu
    │   ├── drivers
    │   └── libc
    └── src
        ├── cpu
        ├── drivers
        └── libc
```