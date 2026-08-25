/* Set the execution entry point of the binary to the label named 'entry' */
ENTRY(entry)

SECTIONS
{
    /* Set the Location Counter (.) to memory address 0x0500.
       All subsequent sections will be loaded starting at this origin. */
    . = 0x0500;

    /* Text section: Place all executable machine code here */
    .text : {
        *(.text)    /* Include the .text section from ALL (*) input object files */
    }

    /* Data section: Place all initialized global and static variables here */
    .data : {
        *(.data)    /* Include the .data section from ALL (*) input object files */
    }

    /* BSS section: Place all uninitialized global and static variables here */
    .bss : {
        *(.bss)     /* Include the .bss section from ALL (*) input object files */
    }

    _kernel_end = .;

    ASSERT(. <= 0x10500, "Error: Kernel is heavier than 64 KBytes (128 sectors)!")
}