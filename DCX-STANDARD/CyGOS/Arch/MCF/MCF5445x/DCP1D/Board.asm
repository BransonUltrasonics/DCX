| $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MCF5445x/DCP1D/Board.asm_v   1.0   19 Apr 2011 11:03:46   PDwivedi  $
|
| Copyright (c) Cyber Group, Inc 2007
| This program is the property of Cyber Group Inc
| Copying of this software is expressly forbidden, without the prior written
| consent of Cyber Group Inc
|
| This file is part of CyGOS-3.
|
| This module implements board and CPU-specific memory initialization. SDRAM
| is initialized and control is transferred to MemoryInit. This code is only
| useful for MultiBoot loader.

				.global _ZN5Board19MultiBootMemoryInitEv

				.text

| Initialize the CPU. This routine may execute in flash and in this case must
| continue in flash.
| Entry register values:
| %D3 - 1 for MultiBoot, 0 for application startup code
| %D4 - flash to SDRAM difference, or 0 for SDRAM execution
| %D5 - 0 for flash, 1 for SDRAM
| %D6 - reset %D0 value
| %D7 - reset %D1 value
_ZN5Board19MultiBootMemoryInitEv:
				| Boot loader code - must initialize CS0 because at startup
				| valid bit is cleared so this chip select is used for all
				| memory accesses. The chip select is configured here...
				| This code is CPU-specific because different families have
				| different preferences about flash and SDRAM mapping...
				move.l	#0xFC008000,%A0
				move.l	#__FLASH_ROM,%D0	| First - base address, this is OK
				move.l	%D0,0(%A0)			| as long as chip select CS0 is not
											| valid, all accesses will use CS0.
|				move.l	#0x003FFDA0,%D0		| CS0 timing, this is OK also as
				move.l	#0x00000D80,%D0		| <- this value for 16-bit boot ROM
				move.l	%D0,8(%A0)			| long as values are reasonable.
				move.l	#0x003F0001,%D0		| Last thing - set the valid bit,
				move.l	%D0,4(%A0)			| just be sure that this does not
											| prevent accessing this code in
											| same place after the CS0 is
											| changed.

				| Here we must initialize SDRAM because no-one has done it
				| Initialize SDRAM controller
				move.l	#0xFC0A4074,%A0 | MSCR_SDRAM
				move.l	#0xAA, %D0
				move.b	%D0,(%A0)

				move.l	#0xFC0B8110,%A0
				move.l	#0x4000001A,%D0
				move.l	%D0,(%A0)		| SDCS0
				move.l	#0xFC0B8114,%A0
				move.l	#0x00000000,%D0	| Disabled
				move.l	%D0,(%A0)		| SDCS1
				move.l	#0xFC0B8008,%A0
				move.l	#0x65311810,%D0
				move.l	%D0,(%A0)		| SDCFG1
				move.l	#0xFC0B800C,%A0
				move.l	#0x59670000,%D0
				move.l	%D0,(%A0)		| SDCFG2
				| Wait a bit
				move.l	#2000,%D0
delay2000:		subq.l	#1,%D0
				bne.b	delay2000
				| Issue PALL
				move.l	#0xFC0B8004,%A0
				move.l	#0xEA0F2002,%D0
				move.l	%D0,(%A0)		| SDCR
				| Issue LEMR
				move.l	#0xFC0B8000,%A0
				move.l	#0x40010408,%D0
				move.l	%D0,(%A0)		| SDMR
				move.l	#0xFC0B8000,%A0
				move.l	#0x00010333,%D0
				move.l	%D0,(%A0)		| SDMR
				| Wait a bit
				move.l	#1000,%D0
delay1000:		subq.l	#1,%D0
				bne.b	delay1000

				| Issue PALL
				move.l	#0xFC0B8004,%A0
				move.l	#0xEA0F2002,%D0
				move.l	%D0,(%A0)		| SDCR
				| Perform two refresh cycles
				move.l	#0xFC0B8004,%A0
				move.l	#0xEA0F2004,%D0
				move.l	%D0,(%A0)		| SDCR
				move.l	#0xFC0B8004,%A0
				move.l	#0xEA0F2004,%D0
				move.l	%D0,(%A0)		| SDCR
				| Clear DLL reset
				move.l	#0xFC0B8000,%A0
				move.l	#0x00010233,%D0
				move.l	%D0,(%A0)		| SDMR
				| refresh enable, DQS_OE
				move.l	#0xFC0B8004,%A0
				move.l	#0x7A0F2C00,%D0
				move.l	%D0,(%A0)		| SDCR
				| Wait a bit
				move.l	#100,%D0
delay100:		subq.l	#1,%D0
				bne.b	delay100

				| Initialize memory mapping
				movea.l	#__SRAM_SP,%SP
				movea.l	#return_mi,%A0
				add.l	%D4,%A0
				move.l	%A0,-(%SP)
				movea.l	#_ZN3MMU4InitEv,%A0
				add.l	%D4,%A0
				jmp		(%A0)
return_mi:
				| Continue with general memory layout initialization...
				movea.l	#MemoryInit,%A0
				add.l	%D4,%A0
				jmp		(%A0)

				.end
