// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/QspiBase.cpp_v   1.0   29 Oct 2010 10:38:28   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares generic QSPI driver for ColdFire processors.

#include "QspiBase.h"
#include "Task.h"
#include "assert.h"

#ifdef MCF_QSPI_QAR

QspiBase::QspiBase(int baseAddress)
{
	regs = (QSPI_Regs *) baseAddress;
}

QspiBase::~QspiBase()
{
	CRASH("Can't destroy QSPI objects");
}

/**
 * InterruptHandler: Handle the interrupt during long transfer.<p>
 * Entry values:
 * <ul>
 * <li>transferLength - word count that must be transferred from QSPI receive
 * buffer into the transfer buffer.
 * <li>transferData - buffer pointer where to transfer the received data.
 * </ul>
 * Processing:
 * <ol>
 * <li>Extract data from QSPI receive buffer one word at a time and place data
 * into transferData buffer. Data length is in variable transferLength, but
 * single transfer may be only 16 words or less. While transferring data, update
 * the data buffer pointer and transferLength variable.
 * <li>If transferLength is 0, all data has been transferred. Disable QSPI
 * interrupt and resume the mutex owner task.
 * <li>If transferLength is not 0, continue data transfer. Extract
 * transferLength words, or 16, whichever is less, from transferData and place
 * data into QSPI transmit buffer. Do not update the data pointer in the
 * transferData buffse so that next QSPI interrupt can place received data into
 * same place.
 * </ol>
 * Exit values:
 * <ul>
 * <li>transferLength - updated to reflect amount of data to transfer.
 * <li>transferData - updated to point to next receiving location in the data
 * buffer.
 * <li>Mutex owner task may be resumed and QSPI interrupts disabled if transfer
 * is complete.
 * </ul>
 */
void QspiBase::IntHandler()
{
	// First, extract received data... Limit transfer amount to 16.
	int length = transferLength;
	if (length > transferOnce)
		length = transferOnce;
	regs->QAR = MCF_QSPI_QAR_ADDR(0) | MCF_QSPI_QAR_RECV;
	while (length-- > 0)
	{
		*transferData++ = regs->QDR;
		transferLength--;
	}
	// Now check if anything is left to send...
	if (transferLength == 0)
	{
		// All done. Disable QSPI interrupt, resume the task, yadda yadda yadda
		regs->QIR &= ~MCF_QSPI_QIR_SPIFE;
		mutex.GetOwnerTask()->Resume();
	}
	else
	{
		length = transferLength;
		if (length > transferOnce)
			length = transferOnce;
		regs->QAR = MCF_QSPI_QAR_ADDR(0) | MCF_QSPI_QAR_TRANS;
		for (int i = 0; i < length; i++)
		{
			regs->QDR = transferData[i];
		}
		regs->QAR = MCF_QSPI_QAR_ADDR(0) | MCF_QSPI_QAR_CMD;
		for (int i = 0; i < length; i++)
		{
			regs->QDR = MCF_QSPI_QDR_BITSE | MCF_QSPI_QDR_DSCK | transferCS;
		}
		regs->QWR = 0
		//		| MCF_QSPI_QWR_WREN
				| MCF_QSPI_QWR_CSIV // chip select active low
				| MCF_QSPI_QWR_ENDQP(length - 1) // last transfer is at 0
				| MCF_QSPI_QWR_NEWQP(0) // first transfer is at 0
				| 0;
		regs->QIR |= 0 | MCF_QSPI_QIR_ABRTL //
				| MCF_QSPI_QIR_WCEF // write collision interrupt reset
				| MCF_QSPI_QIR_ABRT // Abort interrupt reset
				| MCF_QSPI_QIR_SPIF;
		regs->QDLYR |= MCF_QSPI_QDLYR_SPE;
	}
}

/**
 * SingleTransfer: Transfer a single packet of data. Function assumes that QSPI
 * driver is acquired before the function is called. Data is transferred in
 * 16-bit quantities.
 * @param length Transfer length in words.
 * @param data Transfer buffer. At entry the buffer contains transmit data,
 * then the buffer is filled with receive data.
 * @param chipSelect Chip select pin value during the transfer.
 * @param cont : If true CS returns to inactive state after the end of transfer
 * 				 else after every word is transferred.
 */
void QspiBase::SingleTransfer(int length, unsigned short * data,
		int chipSelect, bool cont)
{
	CRASH("DSPI API changed, verify if this matches");
	regs->QIR = 0 | MCF_QSPI_QIR_ABRTL //
			| MCF_QSPI_QIR_WCEF // write collision interrupt reset
			| MCF_QSPI_QIR_ABRT // Abort interrupt reset
			| MCF_QSPI_QIR_SPIF;
	// QSPI finished interrupt reset
	// Transmit data
	regs->QAR = MCF_QSPI_QAR_ADDR(0) | MCF_QSPI_QAR_TRANS;
	for (int i = 0; i < length; i++)
		regs->QDR = data[i];
	// Command Data
	regs->QAR = MCF_QSPI_QAR_ADDR(0) | MCF_QSPI_QAR_CMD;
	for (int i = 0; i < length; i++)
	{
		regs->QDR = MCF_QSPI_QDR_BITSE | MCF_QSPI_QDR_DSCK | (chipSelect << 8)
				| (cont ? MCF_QSPI_QDR_CONT : 0);
	}
	regs->QWR = 0
	//		| MCF_QSPI_QWR_WREN
			| MCF_QSPI_QWR_CSIV // chip select active low
			| MCF_QSPI_QWR_ENDQP(length - 1) // last transfer is at 0
			| MCF_QSPI_QWR_NEWQP(0) // first transfer is at 0
			| 0;
	regs->QDLYR |= MCF_QSPI_QDLYR_SPE;

	// FIXME Indefinite wait
	while (!(regs->QIR & MCF_QSPI_QIR_SPIF))
		;
	regs->QAR = MCF_QSPI_QAR_ADDR(0) | MCF_QSPI_QAR_RECV;
	for (int i = 0; i < length; i++)
		data[i] = regs->QDR;
}

/**
 * LongWordTransfer: Execute long interrupt-driven QSPI data transfer. Function
 * assumes that QSPI is acquired. Current task is suspended while transfer is
 * in progress.
 * @param length Total transfer length, may be more than what fits into QSPI at
 * one time.
 * @param data Transfer buffer, initially filled with transmit data, when
 * transfer is complete, buffer is filled with received data.
 * @param maxLength Maximum transfer length to use in one QSPI transfer.
 * @param chipSelect Chip select pin states for the transfer.
 */
void QspiBase::LongWordTransfer(int length, uint8 * data, int maxLength,
		int chipSelect)
{
	CRASH("DSPI API changed, verify if this matches");
	transferLength = length >> 1;
	transferData = (uint16 *) data;
	transferOnce = maxLength;
	transferCS = chipSelect << 8;
	regs->QIR = 0 | MCF_QSPI_QIR_ABRTL //
			| MCF_QSPI_QIR_WCEF // write collision interrupt reset
			| MCF_QSPI_QIR_ABRT // Abort interrupt reset
			| MCF_QSPI_QIR_SPIF;
	ASSERT((length & 1) == 0);
	int i;
	// This many words must be transferred in first go
	int thisLength = transferLength;
	if (thisLength > transferOnce)
		thisLength = transferOnce;
	// Transmit data
	regs->QAR = MCF_QSPI_QAR_ADDR(0) | MCF_QSPI_QAR_TRANS;
	for (i = 0; i < thisLength; i++)
	{
		regs->QDR = transferData[i];
	}
	// Command Data
	regs->QAR = MCF_QSPI_QAR_ADDR(0) | MCF_QSPI_QAR_CMD;
	for (i = 0; i < thisLength; i++)
	{
		regs->QDR = MCF_QSPI_QDR_BITSE | MCF_QSPI_QDR_DSCK | transferCS;
	}
	regs->QWR = 0
	//		| MCF_QSPI_QWR_WREN
			| MCF_QSPI_QWR_CSIV // chip select active low
			| MCF_QSPI_QWR_ENDQP(thisLength - 1) // last transfer
			| MCF_QSPI_QWR_NEWQP(0) // first transfer is at 0
			| 0;
	// Most likely not needed, but some extra safety. It may be possible that
	// this task is intercepted by higher priority task right after the QSPI
	// interrupt is enabled and before this task suspends, and control gets back
	// to this code after interrupt happens and before this task suspends. In
	// this case task wakes up before it suspends, then it suspends and gets
	// stuck forever in this state. So we enable QSPI interrupts in critical
	// section and execute Suspend() with interrupts disabled. Suspend() will
	// switch to another task and enable CPU interrupts as part of the switch.
	CriticalSection s;
	regs->QIR |= MCF_QSPI_QIR_SPIFE;
	regs->QDLYR |= MCF_QSPI_QDLYR_SPE;
	// We are executing in context of owner task, so here we suspend the task
	// and interrupt handler will resume the task when everything is done.
	mutex.GetOwnerTask()->Suspend();
}

#endif
