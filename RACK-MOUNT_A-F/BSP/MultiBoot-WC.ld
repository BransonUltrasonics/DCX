/*
// $Header: /Aaredcx2010/BransonCyGOS/Arch/MCF/MCF5445x/DCP1C/MultiBoot-DCP1C.ld,v 1.1 2010/12/03 17:06:38 Aare Exp $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This file specifies memory ranges on the target board for RAM (SDRAM)
// configuration.
//
// Linker file defines following symbols:
//
// __VECTOR_TABLE	Beginning of interrupt vector table. All vectors are
//					stored in this table, but only real interrupt vectors
//					must be defined in vectors.asm. Initial SP and PC are
//					specified in this file. The reason to put these longs
//					into here is that 'as' can't handle adding an external
//					offset to a label address.
// __FLASH_RAM		Beginning of flash code image in SDRAM, destination for
//					program copy.
// __FLASH_ROM		Beginning of flash image in flash memory, source for
//					program copy.
// __FLASH_END		End of program and data image in the SDRAM, used to
//					calculate copy image size. Label is same as __DATA_END
//					below.
// __CTOR_START		Constructors for static variables. Label points to an
//					address table. Each module may have a single entry in
//					this table to point at the initialization function.
//					There is maximum one function per module, the function
//					is compiler-generated and it will initialize everything
//					that cannot be initialized at compile time, before
//					main() is executed. Label comes right after the end of
//					executable code.
// __CTOR_END		Pointer past the constructor table described above. If
//					there are no static initialization routines in this
//					table, start and end locations would be same.
// __DATA_START		Beginning of the data area. Label has no specific
//					purpose because all initialized data is copied from ROM
//					to RAM along with the program code. Label is same as
//					__CTOR_END.
// __DATA_END		End of the initialized data area.
// __BSS_START		Beginning of the uninitialized data area. All variables
//					located between this label and __BSS_END will be
//					initialized with 0-s before main() is executed. Label
//					is same as __DATA_END.
// __BSS_END		End of the uninitialized data area.
// __HEAP_START		Beginning of the heap area. Memory can be dynamically
//					allocated and released in this area. Memory area is
//					not initialized with 0-s. Label is same as __BSS_END.
// __HEAP_END		End of the heap area. Label has hard-coded offset from
//					__HEAP_START.
// __STACK_BEGIN	Stack area for the main() routine. Label is same as
//					__HEAP_END.
// __STACK_INIT		End of stack area for the main() routine. Stack is
//					hard-coded to 4K. This should be sufficient for early
//					processing before the RTOS starts.
// __MBAR			Hard-coded to 0x10000000. This is the base location for
//					registers for all on-chip peripherals.
// __FLASH_BASE		Beginning of the flash memory address space. Label is
//					needed to properly set up the start address that will be
//					executed from flash by the boot loader.
// __FLASH_DIFF		Difference in load addresses for flash and SDRAM. This value
//					must be added to any label if the code addressed by this
//					label is located in flash but is executed as if it were in
//					SDRAM. Start label is one of those, and all other labels
//					that are addressed using absolute address before memory is
//					initialized and flash image is copied into SDRAM also belong
//					into this category.
// __USER_FLASH_ROM	User application code location in flash memory.
// __USER_FLASH_RAM User application code location in SDRAM.
// __USER_FLASH_SIZE Maximum user application code size.
//
// Linker file also defines following configuration-specific variables:
//
// __SYSTEM_CLOCK	80		System clock in MHz for this board
// __TERMINAL_PORT	0		Board-specific diagnostics terminal port number
// __TERMINAL_BAUD	115200	Diagnostics terminal baud rate
*/

ENTRY(_MultiBootStart)

MEMORY
{
	/*SDR SDRAM - 128 MBytes*/
	vectors		(RWX) : ORIGIN = 0x50000000, LENGTH = 0x00000400
	config		(RWX) : ORIGIN = 0x50000400, LENGTH = 0x00000100
	sdram		(RWX) : ORIGIN = 0x50000500, LENGTH = 0x07FFFB00
	/* 16MB external Boot Flash */
	boot_flash 	(RWX) : ORIGIN = 0x00000000, LENGTH = 0x01000000 
	boot_rom 	(RWX) : ORIGIN = 0x00040000, LENGTH = 0x00FC0000
	/*128K FRAM*/
    fram      	(RWX) : ORIGIN = 0x0B000000, LENGTH = 0x00020000	
}

SECTIONS
{
	.textvect 0x50000000 :
	{
		__FLASH_RAM		= .;
		__VECTOR_TABLE		= .;
		__INIT_SP		= .;
		LONG(0)
		__INIT_PC		= .;
		LONG(_MultiBootStart + __FLASH_DIFF)
		*(.textvect)
	} > vectors

	.config 0x50000400 :
	{
		. = ALIGN(16);
		__SYSTEM_CLOCK		= .;
		LONG(125)
		__TERMINAL_PORT		= .;
		LONG(0x01)
		__TERMINAL_BAUD		= .;
		LONG(115200)
		__SERIAL_NUMBER		= .;
		LONG(0xFFFFFFFF)
		__ETHERNET_MAC		= .;
		__ETHERNET_MAC0		= .;
		__ETHERNET_MAC1		= .;
		LONG(0xFFFFFFFF)
		SHORT(0xFFFF)
	} > config

	.text 0x50000500 :
	{
		__TEXT_START = .;
		*(.text)
		*(.text.*)
		. = ALIGN(4);
		*(.rodata)
		*(.rodata.*)
		. = ALIGN(4);
		__CTOR_START		= .;
		*(.ctors)
		__CTOR_END			= .;
		. = ALIGN(0x10);
		__TEXT_END			= .;
	} > sdram

	.data 0x51000000 : AT(__TEXT_END)
	{
		. = ALIGN(0x10);
		__DATA_START		= .;
		*(.data)
		. = ALIGN(4);
		__DATA_END		= .;
		__BSS_START		= .;
		*(.bss)
		. = ALIGN(32);
		__BSS_END 		= .;
	} > sdram
	
	___DATA_SIZE = __BSS_END - __DATA_START;

	.fram 0x0B000000 : AT(__TEXT_END + ___DATA_SIZE)
    {
		__FRAM_START = .;
		LONG(0x12345678)
		LONG(0x12345678)/*reserved space for bootloader version*/ 
		LONG(0x12345678)/*reserved space for bootloader version Total 8 bytes*/
		LONG(0x12345678) /*reserved space for bootloader CRC 4 bytes*/
		*(.FRAM)
		. = ALIGN(16);
		__FRAM_END = .;
     } > fram

	__FLASH_END			= __BSS_END;

	__SDRAM				= 0x40000000;
	__SDRAM_SIZE		= 128M;

	/* This stack is used for initialization. main() is also executing in this
	   stack. Stack is allocated 4K, this should be sufficient until application
	   starts CyGOS and then each task will have it's own stack. */  
	/* Initial SP is in the far end of SDRAM */
	__SP_INIT			= __SDRAM_BASE + __SDRAM_SIZE;
	__SP_END			= __SP_INIT - 4K;
	__HEAP_START		= __FLASH_END;
	__HEAP_END			= __SP_END;

	__VECTOR_RAM		= __SDRAM;
	__BOOTLOADER_SIZE	= 256K;
	__USER_FLASH_ROM	= __FLASH_ROM + __BOOTLOADER_SIZE;
	__USER_FLASH_RAM	= __SDRAM_BASE + __BOOTLOADER_SIZE;
	__USER_FLASH_SIZE	= 16M - __BOOTLOADER_SIZE;

	__FLASH_ROM = ORIGIN(boot_flash);
	__FLASH_SIZE = 16M;

	__CORE_SRAM			= 0x80000000;
	__CORE_SRAM_SIZE	= 32K;
	__SRAM_SP			= __CORE_SRAM + __CORE_SRAM_SIZE;
	__SDRAM_BASE		= 0x50000000;
	__FLASH_BASE 		= 0x00000000;
	__FLASH_DIFF		= __FLASH_BASE - __SDRAM_BASE;
	__MMU_BAR			= 0xE1000000;
}
