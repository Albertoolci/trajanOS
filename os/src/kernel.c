void kernel_main() {
    char* vga = (char*) 0xB8000;

    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vga[i]     = ' ';
        vga[i + 1] = 0x0F;
    }

    char* msg = "trajanOS";
    for (int i = 0; msg[i] != '\0'; i++) {
        vga[i * 2]     = msg[i];
        vga[i * 2 + 1] = 0x0F;
    }

    while(1);
}