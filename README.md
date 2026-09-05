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
* **Virtual Memory Manager (`vmm`) & Higher-Half Kernel:** The kernel was migrated from a flat, identity-mapped memory layout to a Higher-Half Kernel (lives in the last virtual 1GB)

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
## Architecture Deep Dive

This section documents, module by module, what is currently implemented in the kernel. It expands on the bullet points from **Current Progress** above without changing them.

### Boot Process (`bootloader/bootloader.asm`)

* The bootloader is a classic 512-byte MBR loaded by the BIOS at `0x7C00` in 16-bit real mode.
* It sets up a minimal real-mode environment (segment registers zeroed, stack pointer at `0x7C00`) and saves the BIOS-provided boot drive (`dl`) for later disk reads.
* Before leaving real mode, it queries the BIOS memory map via `INT 0x15, AX=E820h` and stores the raw entries at physical address `0x8000` (see `E820_LOCATION` in `os/include/mem/e820.h`), so the kernel can later build an accurate physical memory map.
* It then reads the kernel image from disk sector by sector (LBA → CHS translation via `lba_to_chs`) into physical address `0x0050:0x0000` (`0x00000500`), using a sector count (`SECTORS_TO_READ`) that is computed dynamically by the root `Makefile` from the actual size of `kernel.bin`.
* Finally, it far-jumps to `0x0050:0x0000`, handing control to the kernel's own entry point.

### Kernel Entry & Protected Mode (`os/src/kernel_entry.asm`)

* Still in 16-bit real mode, it enables the **A20 line** (I/O port `0x92`) so addresses above 1MB wrap correctly, and loads a minimal **GDT** (null, code, and data segment descriptors) with `lgdt`.
* Sets the `PE` bit in `CR0` and performs a far jump to flush the prefetch queue and switch the CPU into 32-bit protected mode.
* Reloads all segment registers with the new data selector and hands control to `kernel_main()` in `os/src/kernel.c`.

### VGA Text Mode Driver (`os/src/drivers/screen.c`)

* Writes directly to the VGA text buffer at physical `0xB8000` (`VIDEO_ADDRESS`), using 2 bytes per character cell (ASCII byte + colour attribute byte).
* Tracks the hardware cursor position by reading/writing the CRT controller registers (`REG_SCREEN_CTRL` / `REG_SCREEN_DATA`, ports `0x3D4`/`0x3D5`).
* Implements `clear_screen`, `kprint`, `kprint_at`, `backspace`, and a custom `kprintf` supporting the handful of format specifiers the kernel needs (no full libc `printf` semantics), with automatic line scrolling when the last row is filled.

### IDT & Exception Handling (`os/src/cpu/idt.c`, `os/src/cpu/isr.c`, `os/src/cpu/interrupt.asm`)

* `interrupt.asm` defines 32 CPU exception stubs (`isr0`–`isr31`) and 16 hardware IRQ stubs (`irq0`–`irq15`) via NASM macros (`ISR_NOERRCODE`, `ISR_ERRCODE`, `IRQ`). Each stub pushes the interrupt number (and a fake error code when the CPU doesn't push one) and jumps to a common stub that saves the full register state before calling into C.
* `idt.c` builds the 256-entry IDT (`idt_entry_t idt[256]`) and loads it with `lidt`.
* `isr.c` maintains a table of 256 optional C callback pointers (`interrupt_handlers`), dispatches to them from the common stub, and prints a human-readable message (from `exception_messages`) for any of the 32 CPU exceptions that isn't otherwise handled.

### 8259 PIC Remapping (`os/src/cpu/pic.c`)

* Reprograms both the master and slave 8259 PICs so hardware IRQs 0–15 are delivered on IDT vectors 32–47 instead of their default 0–15, avoiding collisions with CPU exceptions.
* Preserves and restores the existing interrupt masks across the remap, and exposes `pic_send_eoi` so every IRQ handler can correctly acknowledge the interrupt (sending EOI to the slave PIC too when `irq >= 8`).

### PIT Timer (`os/src/cpu/timer.c`)

* Programs the 8253/8254 Programmable Interval Timer (ports `0x40`/`0x43`) in mode 3 (square wave generator) to fire at a configurable frequency (100 Hz by default, set from `kernel_main`).
* Registers a callback on IRQ 0 (vector 32) that increments a tick counter, exposed to the rest of the kernel (and to the shell's `ticks` command) via `get_ticks()`.

### PS/2 Keyboard (`os/src/drivers/keyboard.c`)

* Registers a callback on IRQ 1 (vector 33) that reads raw scancodes from I/O port `0x60`, ignores key-release events (top bit set), and translates Set 1 scancodes to ASCII using a lookup table (an ISO Spanish layout is used by default, alongside an unused US layout table for reference).
* Maintains a 256-byte line buffer, handling backspace and Enter, and forwards completed lines to `shell_parse()`.

### Interactive CLI Shell (`os/src/drivers/shell.c`)

* A minimal command-line interpreter that prints a `trajanOS > ` prompt and parses each line typed by the user.
* Currently supports `help`, `clear`, `version`, and `ticks` as built-in commands, plus a fallback "unknown command" message.

### Custom libc Subset (`os/src/libc/`)

* `memory.c` / `memory.h`: freestanding `memcpy` and `memset`.
* `string.c` / `string.h`: the small set of string helpers the kernel needs (`strlen`, `strcmp`, `itoa`, `hex_to_ascii`, etc.), written from scratch since the kernel is built with `-ffreestanding` and has no access to the host's libc.

### Physical Memory Manager — PMM (`os/src/mem/pmm.c`)

* Reads the BIOS E820 map left by the bootloader at `0x8000` to determine total usable RAM, falling back to a conservative 32MB assumption if no valid map is found.
* Tracks memory as 4KB blocks (`PMM_BLOCK_SIZE`) in a bitmap (1 bit per block), placed right after the kernel image at `_kernel_end` (a symbol exported by `os/linker.ld`).
* Marks E820 "usable" regions as free and everything else (including the kernel image itself and the bitmap) as used, exposing `pmm_alloc_block`, `pmm_free_block`, and block-count getters used by `kernel_main` to print total/free/used RAM.

### Virtual Memory Manager & Higher-Half Kernel (`os/src/mem/vmm.c`, `os/linker.ld`, `os/src/kernel_entry.asm`)

The kernel was migrated from a flat, identity-mapped memory layout to a **Higher-Half Kernel**: the kernel's code and data are linked to run at virtual address `0xC0000000` (the classic 3GB split), while user-space (once implemented) would live in the lower `0x00000000`–`0xBFFFFFFF` range. This is a common architecture decision that keeps the kernel's virtual address range fixed and identical across every process/address space, simplifying context switches and inter-process kernel calls.

**Why this isn't a trivial change here:** `bootloader.asm` is a hand-written, non-ELF, non-Multiboot loader — it copies raw sectors into memory and jumps to the first loaded byte in real mode. There is no program-header-aware loader (like GRUB) that could place physical and virtual addresses independently, so the higher-half split has to be built manually with a combination of linker-script tricks and a small assembly trampoline that bootstraps paging before jumping into the higher half.

How it works, end to end:

1. **Two-region linker script (`os/linker.ld`).** The kernel image is split into a low **boot trampoline** (`.boot.text`, containing the 16-/32-bit entry code, the GDT, and a temporary page directory) kept at `VMA == LMA == 0x00000500` — i.e. its linked address and its physical load address are identical, because it runs *before* paging exists — and the rest of the kernel (`.text`, `.rodata`, `.data`, `.bss`), whose sections are linked at `VMA = 0xC0000000 + physical_offset` but stored (`LMA`, via the `AT()` directive) contiguously right after the trampoline. `ld --oformat binary` lays the final flat `kernel.bin` out by `LMA`, so the file on disk stays small and lands exactly where the bootloader expects it physically, while `kernel.elf` (used for GDB symbols) keeps the higher-half `VMA`s for transparent debugging once paging is on.
2. **Temporary page directory (`os/src/kernel_entry.asm`).** Right after entering protected mode (and still executing at the low physical load address), the trampoline builds a page directory using 4MB PSE pages that maps the first 16MB of RAM **twice**: once identity-mapped (`0x00000000 → 0x00000000`, needed because the trampoline's own code, the VGA buffer, and the E820 map all still use raw physical addresses) and once at the higher half (`0xC0000000 → 0x00000000`, where the rest of the kernel is linked to run). It then enables PSE (`CR4`), loads the page directory into `CR3`, and sets the `PG` bit in `CR0`.
3. **The jump into the higher half.** Immediately after enabling paging, a plain relative `jmp` to a higher-half label works correctly: since the low trampoline code is identity-mapped, `EIP` at that point numerically equals its own linked address, so adding the (huge, ~3GB) relative displacement lands `EIP` exactly on the target's higher-half virtual address — which is already mapped to the correct physical frame by step 2.
4. **Definitive kernel stack.** A 16KB stack is reserved in `.bss` (`stack_bottom`/`stack_top`), so it's automatically placed in the higher half; `ESP` is switched to it right after the jump, before calling `kernel_main()`.
5. **Permanent VMM mapping (`vmm_init()` in `os/src/mem/vmm.c`).** Once inside `kernel_main`, `vmm_init()` builds the *definitive* page directory, replicating the same double mapping (identity + higher-half) built by the trampoline, and reloads `CR3`/`CR4`/`CR0` accordingly. Because the identity mapping of the first 16MB is kept alive permanently, `pmm.c` (bitmap placement, E820 reads at `0x8000`) and `screen.c` (VGA buffer at `0xB8000`) keep working completely unmodified, accessing physical memory through their identity-mapped alias.