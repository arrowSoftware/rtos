ENTRY(sysInit)
SECTIONS
{
    . = 1M;

    .boot :
    {
        *(.multiboot_header)
    }
    .text :
    {
        code = .;
        *(.text)
        *(.rodata)
    }
    .data :
    {
        data = .;
        *(.data)
    }
    .bss :
    {
        bss = .;
        *(.bss)
    }
    end = .;
}
