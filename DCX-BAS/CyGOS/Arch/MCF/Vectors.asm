| $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/Vectors.asm_v   1.0   21 Mar 2012 12:27:24   PDwivedi  $
|
| Copyright (c) Cyber Group, Inc 2007
| This program is the property of Cyber Group Inc
| Copying of this software is expressly forbidden, without the prior written
| consent of Cyber Group Inc
|
| This file is part of CyGOS-3.
|
| This module implements interrupt vector table. Interrupt handling is
| processor-specific (FPU, MAC/EMAC presence etc) so first phase of interrupt
| handling is implemented in processor-specific Exception.asm module.
|
| _start label is provided by this module. Linker will take care of providing
| startup PC pointer to start execution from here. This section of code is
| executing from flash, so all jumps must be adjusted to target labels in flash
| memory. Startup code disables interrupts and transfers control to CpuInit
| label in CPU-specific source tree.

				.global	_start
				.global	_RAMstart
				.global	_MultiBootStart

				.section ".textvect"

| Initial values for SP and PC are set by the linker in the linker file, so
| here they must not be present. The reason to move them there was that 'as'
| cannot handle defining words with values 'label + constant', but 'ld' can.

| Another thing: INIT_SP is not used in startup code anywhere. Software gets
| initial value for stack from other sources. So here we cheat a little and
| we use INIT_SP for jumping into startup code as if we were running from
| SDRAM, now that the boot loader has the ability to skip flash burning...

| INIT_SP:		.long	0					| Initial SP, ignored
| INIT_PC:		.long	0					| Initial PC

vector02:		.long	AsmExceptionHandler	| Access Error
vector03:		.long	AsmExceptionHandler	| Address Error
vector04:		.long	AsmExceptionHandler	| Illegal Instruction
vector05:		.long	AsmExceptionHandler	| Reserved
vector06:		.long	AsmExceptionHandler	| Reserved
vector07:		.long	AsmExceptionHandler	| Reserved
vector08:		.long	AsmExceptionHandler	| Privilege Violation
vector09:		.long	AsmExceptionHandler	| Trace
vector0A:		.long	AsmExceptionHandler	| Unimplemented A-Line
vector0B:		.long	AsmExceptionHandler	| Unimplemented F-Line
vector0C:		.long	AsmExceptionHandler	| Debug Interrupt
vector0D:		.long	AsmExceptionHandler	| Reserved
vector0E:		.long	AsmExceptionHandler	| Format Error
vector0F:		.long	AsmExceptionHandler	| Unitialized Int.
vector10:		.long	AsmExceptionHandler	| Reserved
vector11:		.long	AsmExceptionHandler	| Reserved
vector12:		.long	AsmExceptionHandler	| Reserved
vector13:		.long	AsmExceptionHandler	| Reserved
vector14:		.long	AsmExceptionHandler	| Reserved
vector15:		.long	AsmExceptionHandler	| Reserved
vector16:		.long	AsmExceptionHandler	| Reserved
vector17:		.long	AsmExceptionHandler	| Reserved
vector18:		.long	AsmExceptionHandler	| Spurious Interrupt
vector19:		.long	AsmExceptionHandler	| Autovector Level 1
vector1A:		.long	AsmExceptionHandler	| Autovector Level 2
vector1B:		.long	AsmExceptionHandler	| Autovector Level 3
vector1C:		.long	AsmExceptionHandler	| Autovector Level 4
vector1D:		.long	AsmExceptionHandler	| Autovector Level 5
vector1E:		.long	AsmExceptionHandler	| Autovector Level 6
vector1F:		.long	AsmExceptionHandler	| Autovector Level 7
vector20:		.long	AsmExceptionHandler	| TRAP #0
vector21:		.long	AsmExceptionHandler	| TRAP #1
vector22:		.long	AsmExceptionHandler	| TRAP #2
vector23:		.long	AsmExceptionHandler	| TRAP #3
vector24:		.long	AsmExceptionHandler	| TRAP #4
vector25:		.long	AsmExceptionHandler	| TRAP #5
vector26:		.long	AsmExceptionHandler	| TRAP #6
vector27:		.long	AsmExceptionHandler	| TRAP #7
vector28:		.long	AsmExceptionHandler	| TRAP #8
vector29:		.long	AsmExceptionHandler	| TRAP #9
vector2A:		.long	AsmExceptionHandler	| TRAP #10
vector2B:		.long	AsmExceptionHandler	| TRAP #11
vector2C:		.long	AsmExceptionHandler	| TRAP #12
vector2D:		.long	AsmExceptionHandler	| TRAP #13
vector2E:		.long	AsmExceptionHandler	| TRAP #14
vector2F:		.long	AsmExceptionHandler	| TRAP #15
vector30:		.long	AsmExceptionHandler	| Reserved
vector31:		.long	AsmExceptionHandler	| Reserved
vector32:		.long	AsmExceptionHandler	| Reserved
vector33:		.long	AsmExceptionHandler	| Reserved
vector34:		.long	AsmExceptionHandler	| Reserved
vector35:		.long	AsmExceptionHandler	| Reserved
vector36:		.long	AsmExceptionHandler	| Reserved
vector37:		.long	AsmExceptionHandler	| Reserved
vector38:		.long	AsmExceptionHandler	| Reserved
vector39:		.long	AsmExceptionHandler	| Reserved
vector3A:		.long	AsmExceptionHandler	| Reserved
vector3B:		.long	AsmExceptionHandler	| Reserved
vector3C:		.long	AsmExceptionHandler	| Reserved
vector3D:		.long	AsmExceptionHandler	| Reserved
vector3E:		.long	AsmExceptionHandler	| Reserved
vector3F:		.long	AsmExceptionHandler	| Reserved
vector40:		.long	AsmExceptionHandler	| Not used
vector41:		.long	AsmExceptionHandler	| Edge port flag 1
vector42:		.long	AsmExceptionHandler	| Edge port flag 2
vector43:		.long	AsmExceptionHandler	| Edge port flag 3
vector44:		.long	AsmExceptionHandler	| Edge port flag 4
vector45:		.long	AsmExceptionHandler	| Edge port flag 5
vector46:		.long	AsmExceptionHandler	| Edge port flag 6
vector47:		.long	AsmExceptionHandler	| Edge port flag 7
vector48:		.long	AsmExceptionHandler	| Not used
vector49:		.long	AsmExceptionHandler	| Not used
vector4A:		.long	AsmExceptionHandler	| Not used
vector4B:		.long	AsmExceptionHandler	| Not used
vector4C:		.long	AsmExceptionHandler	| Not used
vector4D:		.long	AsmExceptionHandler	| Not used
vector4E:		.long	AsmExceptionHandler	| Not used
vector4F:		.long	AsmExceptionHandler	| USB Endpoint 0
vector50:		.long	AsmExceptionHandler	| USB Endpoint 1
vector51:		.long	AsmExceptionHandler	| USB Endpoint 2
vector52:		.long	AsmExceptionHandler	| USB Endpoint 3
vector53:		.long	AsmExceptionHandler	| USB Endpoint 4
vector54:		.long	AsmExceptionHandler	| USB Endpoint 5
vector55:		.long	AsmExceptionHandler	| USB Endpoint 6
vector56:		.long	AsmExceptionHandler	| USB 2.0 general interrupt
vector57:		.long	AsmExceptionHandler	| USB 2.0 core interrupt
vector58:		.long	AsmExceptionHandler	| OR of all USB interrupts
vector59:		.long	AsmExceptionHandler	| DSPI over/underflow
vector5A:		.long	AsmExceptionHandler	| DSPI RxFIFO overflow
vector5B:		.long	AsmExceptionHandler	| DSPI RxFIFO drain
vector5C:		.long	AsmExceptionHandler	| DSPI TxFIFO underflow
vector5D:		.long	AsmExceptionHandler	| DSPI transfer complete
vector5E:		.long	AsmExceptionHandler	| DSPI TxFIFO fill
vector5F:		.long	AsmExceptionHandler	| DSPI end of queue
vector60:		.long	AsmExceptionHandler	| PSC3
vector61:		.long	AsmExceptionHandler	| PSC2
vector62:		.long	AsmExceptionHandler	| PSC1
vector63:		.long	AsmExceptionHandler	| PSC0
vector64:		.long	AsmExceptionHandler	| Combined comm timers
vector65:		.long	AsmExceptionHandler	| SEC
vector66:		.long	AsmExceptionHandler	| FEC1
vector67:		.long	AsmExceptionHandler	| FEC0
vector68:		.long	AsmExceptionHandler	| I2C
vector69:		.long	AsmExceptionHandler	| PCI arbiter
vector6A:		.long	AsmExceptionHandler	| Comm bus PCI
vector6B:		.long	AsmExceptionHandler	| XLB PCI
vector6C:		.long	AsmExceptionHandler	| Not used
vector6D:		.long	AsmExceptionHandler	| Not used
vector6E:		.long	AsmExceptionHandler	| Not used
vector6F:		.long	AsmExceptionHandler	| XLB_ARB
vector70:		.long	AsmExceptionHandler	| MCDMA
vector71:		.long	AsmExceptionHandler	| CAN0 ERROR
vector72:		.long	AsmExceptionHandler	| CAN0 BUSOFF
vector73:		.long	AsmExceptionHandler	| CAN0 MBOR
vector74:		.long	AsmExceptionHandler	| Not used
vector75:		.long	AsmExceptionHandler	| SLT1
vector76:		.long	AsmExceptionHandler	| SLT0
vector77:		.long	AsmExceptionHandler	| CAN1 ERROR
vector78:		.long	AsmExceptionHandler	| CAN1 BUSOFF
vector79:		.long	AsmExceptionHandler	| CAN1 MBOR
vector7A:		.long	AsmExceptionHandler	| Not used
vector7B:		.long	AsmExceptionHandler	| GPT3
vector7C:		.long	AsmExceptionHandler	| GPT2
vector7D:		.long	AsmExceptionHandler	| GPT1
vector7E:		.long	AsmExceptionHandler	| GPT0
vector7F:		.long	AsmExceptionHandler	| Not used
vector80:		.long	AsmExceptionHandler
vector81:		.long	AsmExceptionHandler
vector82:		.long	AsmExceptionHandler
vector83:		.long	AsmExceptionHandler
vector84:		.long	AsmExceptionHandler
vector85:		.long	AsmExceptionHandler
vector86:		.long	AsmExceptionHandler
vector87:		.long	AsmExceptionHandler
vector88:		.long	AsmExceptionHandler
vector89:		.long	AsmExceptionHandler
vector8A:		.long	AsmExceptionHandler
vector8B:		.long	AsmExceptionHandler
vector8C:		.long	AsmExceptionHandler
vector8D:		.long	AsmExceptionHandler
vector8E:		.long	AsmExceptionHandler
vector8F:		.long	AsmExceptionHandler
vector90:		.long	AsmExceptionHandler
vector91:		.long	AsmExceptionHandler
vector92:		.long	AsmExceptionHandler
vector93:		.long	AsmExceptionHandler
vector94:		.long	AsmExceptionHandler
vector95:		.long	AsmExceptionHandler
vector96:		.long	AsmExceptionHandler
vector97:		.long	AsmExceptionHandler
vector98:		.long	AsmExceptionHandler
vector99:		.long	AsmExceptionHandler
vector9A:		.long	AsmExceptionHandler
vector9B:		.long	AsmExceptionHandler
vector9C:		.long	AsmExceptionHandler
vector9D:		.long	AsmExceptionHandler
vector9E:		.long	AsmExceptionHandler
vector9F:		.long	AsmExceptionHandler
vectorA0:		.long	AsmExceptionHandler
vectorA1:		.long	AsmExceptionHandler
vectorA2:		.long	AsmExceptionHandler
vectorA3:		.long	AsmExceptionHandler
vectorA4:		.long	AsmExceptionHandler
vectorA5:		.long	AsmExceptionHandler
vectorA6:		.long	AsmExceptionHandler
vectorA7:		.long	AsmExceptionHandler
vectorA8:		.long	AsmExceptionHandler
vectorA9:		.long	AsmExceptionHandler
vectorAA:		.long	AsmExceptionHandler
vectorAB:		.long	AsmExceptionHandler
vectorAC:		.long	AsmExceptionHandler
vectorAD:		.long	AsmExceptionHandler
vectorAE:		.long	AsmExceptionHandler
vectorAF:		.long	AsmExceptionHandler
vectorB0:		.long	AsmExceptionHandler
vectorB1:		.long	AsmExceptionHandler
vectorB2:		.long	AsmExceptionHandler
vectorB3:		.long	AsmExceptionHandler
vectorB4:		.long	AsmExceptionHandler
vectorB5:		.long	AsmExceptionHandler
vectorB6:		.long	AsmExceptionHandler
vectorB7:		.long	AsmExceptionHandler
vectorB8:		.long	AsmExceptionHandler
vectorB9:		.long	AsmExceptionHandler
vectorBA:		.long	AsmExceptionHandler
vectorBB:		.long	AsmExceptionHandler
vectorBC:		.long	AsmExceptionHandler
vectorBD:		.long	AsmExceptionHandler
vectorBE:		.long	AsmExceptionHandler
vectorBF:		.long	AsmExceptionHandler
vectorC0:		.long	AsmExceptionHandler
vectorC1:		.long	AsmExceptionHandler
vectorC2:		.long	AsmExceptionHandler
vectorC3:		.long	AsmExceptionHandler
vectorC4:		.long	AsmExceptionHandler
vectorC5:		.long	AsmExceptionHandler
vectorC6:		.long	AsmExceptionHandler
vectorC7:		.long	AsmExceptionHandler
vectorC8:		.long	AsmExceptionHandler
vectorC9:		.long	AsmExceptionHandler
vectorCA:		.long	AsmExceptionHandler
vectorCB:		.long	AsmExceptionHandler
vectorCC:		.long	AsmExceptionHandler
vectorCD:		.long	AsmExceptionHandler
vectorCE:		.long	AsmExceptionHandler
vectorCF:		.long	AsmExceptionHandler
vectorD0:		.long	AsmExceptionHandler
vectorD1:		.long	AsmExceptionHandler
vectorD2:		.long	AsmExceptionHandler
vectorD3:		.long	AsmExceptionHandler
vectorD4:		.long	AsmExceptionHandler
vectorD5:		.long	AsmExceptionHandler
vectorD6:		.long	AsmExceptionHandler
vectorD7:		.long	AsmExceptionHandler
vectorD8:		.long	AsmExceptionHandler
vectorD9:		.long	AsmExceptionHandler
vectorDA:		.long	AsmExceptionHandler
vectorDB:		.long	AsmExceptionHandler
vectorDC:		.long	AsmExceptionHandler
vectorDD:		.long	AsmExceptionHandler
vectorDE:		.long	AsmExceptionHandler
vectorDF:		.long	AsmExceptionHandler
vectorE0:		.long	AsmExceptionHandler
vectorE1:		.long	AsmExceptionHandler
vectorE2:		.long	AsmExceptionHandler
vectorE3:		.long	AsmExceptionHandler
vectorE4:		.long	AsmExceptionHandler
vectorE5:		.long	AsmExceptionHandler
vectorE6:		.long	AsmExceptionHandler
vectorE7:		.long	AsmExceptionHandler
vectorE8:		.long	AsmExceptionHandler
vectorE9:		.long	AsmExceptionHandler
vectorEA:		.long	AsmExceptionHandler
vectorEB:		.long	AsmExceptionHandler
vectorEC:		.long	AsmExceptionHandler
vectorED:		.long	AsmExceptionHandler
vectorEE:		.long	AsmExceptionHandler
vectorEF:		.long	AsmExceptionHandler
vectorF0:		.long	AsmExceptionHandler
vectorF1:		.long	AsmExceptionHandler
vectorF2:		.long	AsmExceptionHandler
vectorF3:		.long	AsmExceptionHandler
vectorF4:		.long	AsmExceptionHandler
vectorF5:		.long	AsmExceptionHandler
vectorF6:		.long	AsmExceptionHandler
vectorF7:		.long	AsmExceptionHandler
vectorF8:		.long	AsmExceptionHandler
vectorF9:		.long	AsmExceptionHandler
vectorFA:		.long	AsmExceptionHandler
vectorFB:		.long	AsmExceptionHandler
vectorFC:		.long	AsmExceptionHandler
vectorFD:		.long	AsmExceptionHandler
vectorFE:		.long	AsmExceptionHandler
vectorFF:		.long	AsmExceptionHandler

				.text

| This label must be here so that linker uses it as a start of execution, and
| also the entry point here will make it easier to see how things go for
| different microprocessors. This label transfers execution to one of many
| CPU-specific routines that are provided for each platform.
| Registers used:
| %A7 - CPU::Init label where to jump, in flash or SDRAM
| %D3 - 1 for MultiBoot, 0 for application startup code
| %D4 - flash to SDRAM difference, or 0 for SDRAM execution
| %D5 - 0 for flash, 1 for SDRAM
| %D6 - reset %D0 value
| %D7 - reset %D1 value

_start:			move.w	#0x2700,%SR
				moveq.l	#0,%D5				| Flash execution
				move.l	#__FLASH_DIFF,%D4
				bra.b	CommonStart

_RAMstart:		move.w	#0x2700,%SR
				moveq.l	#1,%D5				| SDRAM execution
				moveq.l	#0,%D4

CommonStart:	move.l	%D0,%D6				| Save boot CPU ID registers for
				move.l	%D1,%D7				| later use.
				clr.l	%D3
				movea.l	#_ZN3CPU4InitEv,%A7
				add.l	%D4,%A7
				jmp		(%A7)

_MultiBootStart:
				move.w	#0x2700,%SR
				moveq.l	#0,%D5				| Flash execution
				move.l	#__FLASH_DIFF,%D4
				move.l	%D0,%D6				| Save boot CPU ID registers for
				move.l	%D1,%D7				| later use.
				moveq.l	#1,%D3
				movea.l	#_ZN3CPU13MultiBootInitEv,%A7
				add.l	%D4,%A7
				jmp		(%A7)

				.end
