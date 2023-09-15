| $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MemoryInit.asm_v   1.0   29 Oct 2010 10:38:28   ygupta  $
|
| Copyright (c) Cyber Group, Inc 2007
| This program is the property of Cyber Group Inc
| Copying of this software is expressly forbidden, without the prior written
| consent of Cyber Group Inc
|
| This file is part of CyGOS-3.
|
| This module provides a routine to copy executable code from flash memory into
| SDRAM. After the copy code is executed, control is transferred to board-
| specific initialization code in SDRAM.
|
| This module copies flash image into RAM, including interrupt vector table,
| program code and initialized data. Uninitialized data is not copied. Vector
| table is also copied into appropriate place in SDRAM and vector base register
| is set to point to the table.
|
| MemoryInit may also be executed from SDRAM. In this case D4 contains 0 and D5
| contains 1. If code is already in SDRAM, copy is not executed. Interrupt
| table is still initialized and BSS is cleared.

				.global MemoryInit

				.text

| Entry register values:
| %D3 - 1 for MultiBoot, 0 for application startup code
| %D4 - flash to SDRAM difference, or 0 for SDRAM execution
| %D5 - 0 for flash, 1 for SDRAM
| %D6 - reset %D0 value
| %D7 - reset %D1 value
MemoryInit:
				| Check if we are in flash...
				tst.l	%D5
				bne.b	SkipFlashCopy
				| OK, so we are in flash. Copy the code to SDRAM. This code
				| includes .text and .data sections, but no .bss
				move.l	#__FLASH_RAM,%A0		| to
				move.l	#__FLASH_ROM,%A1		| from
				move.l	#__FLASH_END,%D0
				sub.l	#__FLASH_RAM,%D0		| length in bytes
				lsr.l	#2,%D0					| now in longs
CopyLoop:		move.l	(%A1)+,(%A0)+
				subq.l	#1,%D0
				bne.b	CopyLoop
SkipFlashCopy:
				| Interrupt vector table is next
				move.l	#__VECTOR_RAM,%A0		| to
				move.l	#__VECTOR_TABLE,%A1		| from
				move.l	#256,%D0
CopyLoop1:		move.l	(%A1)+,(%A0)+
				subq.l	#1,%D0
				bne.b	CopyLoop1
				move.l	#__VECTOR_RAM,%D0
				movec	%D0,%VBR
				| Clear BSS
				move.l	#__BSS_START,%A0		| to
				move.l	#0,%D1					| this is zero
				move.l	#__BSS_END,%D0
				sub.l	#__BSS_START,%D0		| length in bytes
				lsr.l	#2,%D0					| now in longs
				beq.b		SkipBss
CopyLoop2:		move.l	%D1,(%A0)+
				subq.l	#1,%D0
				bne.b	CopyLoop2
SkipBss:
				| All memory initialized, we can start using the main stack...
				move.l	#__SP_INIT,%SP

				| SDRAM is all in place, put reset register values into right
				| locations.
				move.l	%D6,_ZN3CPU7ResetD0E
				move.l	%D7,_ZN3CPU7ResetD1E
				| Now that real code is in real place and properly initialized,
				| jump into it. Execution will continue from SDRAM.
				move.l	%D3,-(%SP)
				jsr		_ZN5Board4InitEb
				move.l	(%SP)+,%D3
				| Execute the constructors for global objects
				lea.l	__CTOR_START,%A2
				lea.l	__CTOR_END,%A3
ctor_loop:		cmpa.l	%A2,%A3
				beq		ctor_exit
				move.l	(%A2)+,%A0
				jsr		(%A0)
				bra		ctor_loop
ctor_exit:
				| Now it's time to call main and hope it does not return...
				jsr		main
				bra		.

				.end
