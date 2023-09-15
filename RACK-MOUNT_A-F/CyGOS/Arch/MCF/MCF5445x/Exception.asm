| $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Arch/MCF/MCF5445x/Exception.asm_v   1.1   24 Jul 2015 08:58:18   sesharma  $
|
| Copyright (c) Cyber Group, Inc 2007
| This program is the property of Cyber Group Inc
| Copying of this software is expressly forbidden, without the prior written
| consent of Cyber Group Inc
|
| This file is part of CyGOS-3.
|
| This module implements first phase of interrupt handling. Interrupt handler
| saves all registers that may be modified by the C++ or assembly code (D0-D1,
| A0-A1, and EMAC registers that are not saved by the compiler across function
| calls), then it calls ExceptionHandler() routine and schedules highest
| priority task to run when interrupt handler suspends or resumes some tasks.
|
| Current design does not support nested interrupts. Support is easy to add
| and may be required later.
|
| FIXME EMAC is not supported yet.

				.global AsmExceptionHandler

AsmExceptionHandler:
				| disable all interrupts! we don't want the timer interrupt
				| to interfere with peripheral interrupts
				move.w	#0x2700,%SR
				| now address/data
				lea		-16(%SP),%SP
				movem.l	%D0/%D1/%A0/%A1,(%SP)
				| now call C interrupt handler
				lea		16(%SP),%A1		| <- this is the original exception
										|    frame address
				move.l	%A1,-(%SP)
				jsr		_ZN9Exception12FirstHandlerEPv
				lea		4(%SP),%SP
				| OK, here we are... do we need to switch tasks?
				move.l	_ZN4RTOS11taskCurrentE,%D0
				cmp.l	_ZN4RTOS11taskHighestE,%D0
				| flags will not be changed from here so we'll restore
				| everything saved by the entry to the interrupt handler
				| address/data first
				movem.l	(%SP),%A0/%A1/%D0/%D1
				lea		16(%SP),%SP
				| flags are still there, not-zero if task switch is needed
				bne.b	switch_tasks
				rte

| This label switches tasks. Current and highest task pointers are set up, so
| all we need to do is to save context, switch stacks and restore contexts.
| This is done by RTOS as well, when executing TaskSwitch function. So in here
| we jump into TaskSwitch function instead of duplicating the code.
switch_tasks:
				jmp		_ZN4RTOS10TaskSwitchEv

				.end
