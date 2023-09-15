| $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Arch/MCF/MCF5445x/MCF5445xRTOS.asm_v   1.1   24 Jul 2015 08:58:20   sesharma  $
|
| Copyright (c) Cyber Group, Inc 2007
| This program is the property of Cyber Group Inc
| Copying of this software is expressly forbidden, without the prior written
| consent of Cyber Group Inc
|
| This file is part of CyGOS-3.
|
| This module implements RTOS assembly routines. Module is processor-specific.
| Stack layout in this module must match stack layout created in
| Task::Initialize function.

				.global _ZN4RTOS10TaskSwitchEv
				.global _ZN4RTOS16ExecuteFirstTaskEv

				.text

| TaskSwitch: Function handles task switch when <b>Schedule</b> function sets
| up new task variables. Function address must be stored directly into the
| primary interrupt vector table at VBR, entry for <b>TRAP 15</b> exception,
| without going through the normal exception handling process. RTOS
| constructor sets the pointer in right place to point to this function.
| <p>There is another function that does almost the same thing -
| DoTaskSwitch(). DoTaskSwitch() function is designed to be called from
| interrupt handler, Trap15 is the interrupt handler to be executed from user
| code through Schedule() function.
| @@see #Schedule
| @@see #DoTaskSwitch
_ZN4RTOS10TaskSwitchEv:
				lea		-60(%SP),%SP
				movem.l	%A0-%A6/%D0-%D7,(%SP)
				move.l	_ZN4RTOS11taskCurrentE,%A0
				move.l	%SP,4(%A0)			| refer to TaskBase
_ZN4RTOS16ExecuteFirstTaskEv:
				move.l	_ZN4RTOS11taskHighestE,%A0
				move.l	%A0,_ZN4RTOS11taskCurrentE
				| switch to new task stack
				move.l	4(%A0),%SP
				| restore registers from new stack and return
				movem.l	(%SP),%A0-%A6/%D0-%D7
				lea		60(%SP),%SP
				rte

				.end
