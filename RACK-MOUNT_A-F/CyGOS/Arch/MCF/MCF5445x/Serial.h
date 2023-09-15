// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Arch/MCF/MCF5445x/Serial.h_v   1.0   09 Jun 2015 09:13:30   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares Serial object. Object is capable of 8-bit serial
// communication.

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "FIFO.h"

class Task;

/**
 * class Serial: Serial interface driver. This interface is interrupt-driven
 * and can be only used from tasks that execute when RTOS is running.
 */
class Serial
{
public:
	Serial(int rxbufsize, int txbufsize);
	void Init(int ch, int baudrate, int irqlevel, int prio, bool flowcontrol);
	bool Send(Task * sender, unsigned char ch, int timeout);
	bool Receive(Task * receiver, unsigned char & ch, int timeout);
	void FlushOutputBuffer();

protected:
	static void IntHandler(int level, void * frame);
	void IntHandler();
	void WriteIMR(int value);
	void EnableUARTInterrupt();
	/**
	 * ResetIMR: Reset some bits in IMR.
	 * @param value Bits to reset. Bits must be 1 to be reset.
	 */
	void ResetIMR(int value)
	{
		WriteIMR(ReadIMR() & ~value);
	}
	/**
	 * SetIMR: Set some bits in IMR.
	 * @param value Bits to set. All others are not changed.
	 */
	void SetIMR(int value)
	{
		WriteIMR(ReadIMR() | value);
	}
	/**
	 * ReadIMR: Read the shadow copy of IMR.
	 * @return short Last value written into IMR.
	 */
	int ReadIMR() const
	{
		return imr;
	}

	/**
	 * rxbuf: Pointer to the receiver FIFO.
	 */
	Fifo<unsigned char> * rxbuf;
	/**
	 * txbuf: Pointer to the transmitter FIFO.
	 */
	Fifo<unsigned char> * txbuf;
	/**
	 * port: Port number, 0...2
	 */
	int port;
	/**
	 * flowcontrol: True if flow control is o be used, false if not.
	 * @@deprecated Flow control is not implemented.
	 */
	bool flowcontrol;
	/**
	 * imr: Serial port interrupt mask register value.
	 */
	int imr;
	static Serial * serial[3];
};

#endif
