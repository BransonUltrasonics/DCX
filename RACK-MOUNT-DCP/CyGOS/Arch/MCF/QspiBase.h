// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Arch/MCF/QspiBase.h_v   1.0   18 Aug 2014 09:15:28   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares generic QSPI driver for ColdFire processors.

#ifndef QSPIBASE_H_
#define QSPIBASE_H_

#include "SPI.h"
#include "ArchDefs.h"

struct QSPI_Regs
{
public:
	uint16 QMR;
private:
	uint16 unused02;
public:
	uint16 QDLYR;
private:
	uint16 unused06;
public:
	uint16 QWR;
private:
	uint16 unused0A;
public:
	uint16 QIR;
private:
	uint16 unused0E;
public:
	uint16 QAR;
private:
	uint16 unused12;
public:
	uint16 QDR;
private:
	uint16 unused16;
};

class QspiBase: public SPI
{
protected:
	enum Configuration
	{
		StackSize = 128,
	};
public:
	QspiBase(int baseAddress);
	virtual ~QspiBase();
protected:
	void SingleTransfer(int length, uint16 * data, int chipSelect, bool cont);
	void LongWordTransfer(int length, uint8 * data, int maxLength,
			int chipSelect);
	void IntHandler();
	volatile QSPI_Regs * regs;
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
};

#endif
