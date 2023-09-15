// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/RTOS/PrintfTask.h_v   1.0   18 Aug 2014 09:16:44   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares Printf task object.

#ifndef PRINTFTASK_H_
#define PRINTFTASK_H_

#include "Task.h"
#include "Serial.h"
#include "CyGOS.h"

/**
 * class PrintfTask: Interrupt-driven buffered support for the printf()
 * function. Each printf() call will work as usually, using polled terminal
 * output, before RTOS starts. When RTOS starts, Terminal::Out function will
 * use this task to queue up all characters from the single function call and
 * then printf() will let this task know that the buffer is available for the
 * data to be sent to serial port. This task will extract all data from the
 * queue and send it to serial port in one go, so multiple printf() calls from
 * different tasks will not interfere with each other and each printf() output
 * comes out undisturbed.
 */
class PrintfTask: public Task
{
	friend int getcharWait(int timeout);
public:
	PrintfTask();
	void Print(unsigned char ch);
	/**
	 * FlushOutputBuffer: Flush the serial port output buffer. Function executes
	 * most likely with interrupts disabled and returns when all data from serial
	 * port is transmitted.
	 */
	void FlushOutputBuffer()
	{
		port.FlushOutputBuffer();
	}

protected:
	void Run();
	/**
	 * queue: The queue is used to accumulate data that single printf() call
	 * is sending to the serial port. When task runs, data from this FIFO is
	 * extracted and sent to serial port in one go.
	 */
	Fifo<unsigned char> queue;
	/**
	 * port: This serial port is used for output. It is the same as the one
	 * used for polled terminal I/O before RTOS starts. Serial port
	 * initialization code has few checks for this and will not scramble serial
	 * port configuration when the task is started, so anything printf-ed before
	 * RTOS starts should still come out fine when the RTOS is running.
	 */
	Serial port;
};

#endif
