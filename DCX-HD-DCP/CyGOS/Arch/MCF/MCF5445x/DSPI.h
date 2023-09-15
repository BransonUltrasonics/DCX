// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MCF5445x/DSPI.h_v   1.1   28 Mar 2012 10:57:36   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares DSPI driver for MCF54455 processor.

#ifndef DSPI_H_
#define DSPI_H_

#include "CPUDefs.h"

#include "SPI.h"

struct DSPI_Regs
{
	uint32 MCR; // DSPI module configuration register (DSPI_MCR)
	uint32 unused04;
	uint32 TCR; // DSPI transfer count register (DSPI_TCR)
	uint32 CTAR[8]; // DSPI clock and transfer attributes registers (DSPI_CTARn)
	uint32 SR; // DSPI status register (DSPI_SR)
	uint32 RSER; // DSPI DMA/interrupt request select and enable register
	uint32 PUSHR; // DSPI push TX FIFO register (DSPI_PUSHR)
	uint32 POPR; // DSPI pop RX FIFO register (DSPI_POPR)
	uint32 TXFR[16]; // DSPI transmit FIFO registers (DSPI_TXFRn)
	uint32 RXFR[16]; // DSPI receive FIFO registers (DSPI_RXFRn)
};

class DSPI: public SPI
{
	volatile DSPI_Regs * reg;
public:
	DSPI(int instance, int irqlevel, int irqprio);
	~DSPI();
	/**
	 * 	Bit-bang
	 */
	void EnableGPIO();
	void SetSignals(int clock, int output, int cs);
	int ReadInput();
	/**
	 * 	Regular SPI Master mode
	 */
	void EnableSPI(bool phase, int bits, int clock, bool polarity, bool lsb);
	unsigned char ReadByte(unsigned char send, int chipSelect);
	void SingleTransfer(int length, uint16 * data, int chipSelect, bool cont);
	void LongWordTransfer(int length, uint8 * data, int maxLength,
			int chipSelect);
	/**
	 * 	Variables
	 */
	static DSPI * thisPtr;
protected:
	static void InterruptHandler(int level, void * frame);
	void IntHandler();
	uint32 GetBaudRate(uint32 clock, uint32 baud, bool pol, bool pha);
	void ConfigureSPIPins();
	void EnableSPI()
	{
		reg->MCR &= ~MCF_DSPI_DSPI_MCR_HALT;
	}
	void DisableSPI()
	{
		reg->MCR |= MCF_DSPI_DSPI_MCR_HALT;
	}
	/**
	 * transferLength: This many words must be transferred in one interrupt-
	 * driven long transfer. The assumption is that long transfers are always
	 * multiples of 16 bits.
	 */
	int transferLength;
	/**
	 * transferOnce: This many words can be transferred in each phase of long
	 * interrupt-driven transfer. Transfer size may be less than this number,
	 * but not more than that.
	 */
	int transferOnce;
	/**
	 * transferData: Pointer to a buffer containing data for one long interrupt-
	 * driven transfer. Buffer is pre-filled with transmit data. When transfer
	 * is completed, same buffer will contain received data.
	 */
	uint16 * transferData;
	/**
	 * transferCS: This chip select value is used for long interrupt-driven
	 * transfers. The value is already shifted 8 bits so it goes right into
	 * place without extra work.
	 */
	int transferCS;

	//current
	uint32 baud;
};

#endif
