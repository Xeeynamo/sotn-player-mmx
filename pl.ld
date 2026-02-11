SECTIONS {
    . = 0x8013C000;
    .data : {
        KEEP(*(.data.MMX_player))
        *(.data*)
    }
    .rodata : {
        *(.rodata*)
    }
    .text : {
        *(.text*)
    }
    end = .;
}
