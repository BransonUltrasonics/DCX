| $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Arch/MCF/MCF5445x/CPU.asm_v   1.1   24 Jul 2015 08:58:18   sesharma  $
|
| Copyright (c) Cyber Group, Inc 2007
| This program is the property of Cyber Group Inc
| Copying of this software is expressly forbidden, without the prior written
| consent of Cyber Group Inc
|
| This file is part of CyGOS-3.
|
| This module implements CPU-specific initialization. Main core features are
| implemented here, such as base addresses for CPU static memory blocks and
| base address for internal peripheral device access. Peripheral devices are
| initialized in separate modules as appropriate. When this code is executed,
| SDRAM is already initialized by the boot loader specific to the target board.
| After CPU is initialized, control is transferred to memory initialization
| routine which is still located in flash memory.
|
| CPU cache, for example, is not initialized here even if the initialization
| code is CPU-specific. The reason here is that the cacheable regions are
| board-specific. Board initialization code will initialize cache areas.

				.global _ZN3CPU4InitEv
				.global _ZN3CPU13MultiBootInitEv
				.global	_ZN3CPU11SwitchStackEPv

				.text

| Initialize the CPU. This routine may execute in flash and in this case must
| continue in flash.
| Entry register values:
| %D3 - 1 for MultiBoot, 0 for application startup code
| %D4 - flash to SDRAM difference, or 0 for SDRAM execution
| %D5 - 0 for flash, 1 for SDRAM
| %D6 - reset %D0 value
| %D7 - reset %D1 value
_ZN3CPU4InitEv:
				| Initialize RAMBAR - locate it on the data bus
				move.l	#__CORE_SRAM,%D0
				add.l	#0x201,%D0
				movec	%D0,%RAMBAR
				| Continue with board-specific initialization...
				movea.l	#MemoryInit,%A7
				add.l	%D4,%A7
				jmp		(%A7)

_ZN3CPU13MultiBootInitEv:
				| Initialize RAMBAR - locate it on the data bus
				move.l	#__CORE_SRAM,%D0
				add.l	#0x201,%D0
				movec	%D0,%RAMBAR
				| Now RAM is initialized, continue with initialization
				| Continue with board-specific initialization...
				movea.l	#_ZN5Board19MultiBootMemoryInitEv,%A7
				add.l	%D4,%A7
				jmp		(%A7)

_ZN3CPU11SwitchStackEPv:
				move.l	0(%SP),%A1			| return address
				move.l	4(%SP),%A0			| new stack pointer
				move.l	%A0,%SP				| switch to new stack
				jmp		(%A1)				| return

				.end
