/* bootloader.ld */
ENTRY(Reset_Handler)

_estack = 0x20020000;    /* End of RAM (128KB) */

MEMORY
{
    /* CRITICAL: Sector 0 is exactly 16KB (0x4000 bytes) */
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 16K
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)           /* Code */
        *(.text*)
        *(.rodata)         /* Constants */
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } >FLASH

    /* Initialized Data (Global vars) - Loads in Flash, runs in RAM */
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT> FLASH

    /* Uninitialized Data (Zeros) */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM
}