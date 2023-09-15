// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Devices/AT93C66.cpp_v   1.0   24 Jul 2015 09:16:32   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements AT93C66 driver.

#include "AT93C66.h"

#define OP_OTHER (0)
#define OP_WRITE (1 << 8)
#define OP_READ (1 << 9)
#define OP_ERASE (3 << 8)
#define EW_EN (3 << 6)
#define EW_DS (0)
#define ER_AL (1 << 7)
#define WR_AL (1 << 6)
#define AT93C66ADDRESS_MASK 0xFF
#define START_BIT (1 << 10)
#define AT93C66_BAUD 1000000


AT93C66::AT93C66(SPI * spi, int cs, int ctar)
{
	this->spi = spi;
	this->cs = 1 << cs;
	this->ctar = ctar;
	spi->EnableSPI(false, 12, AT93C66_BAUD, false, false, ctar);
	printf("\n %s cs %d ctar %d ", __FUNCTION__, cs , ctar);
}

AT93C66::~AT93C66()
{

}

void AT93C66::Write(uint16 address, uint16 data)
{
	uint16 msg[3];
	//0000 0000 0000 0000
	//      101
	msg[0] = 0 | START_BIT | OP_WRITE | (address & AT93C66ADDRESS_MASK);
	msg[1] = (data >> 4);
	msg[2] = (data << 8);
	SpiLock q(spi);
	q.Lock(-1);
	spi->SingleTransfer(3, msg, cs, false, ctar);
	q.UnLock();
}

uint16 AT93C66::Read(uint16 address)
{
	uint16 msg[3];
	//0000 0000 0000 0000
	//      110
	msg[0] = 0 | START_BIT | OP_READ | (address & AT93C66ADDRESS_MASK);
	msg[1] = 0; //Needed to keep CS asserted low while reading
	msg[2] = 0;
	SpiLock q(spi);
	q.Lock(-1);
	spi->SingleTransfer(3, msg, cs, false, ctar);
	q.UnLock();
	return ((msg[1] << 5) | (msg[2] >> 7));
}

void AT93C66::WriteEnable(bool enable)
{
	uint16 msg[1];
	if (enable)
		// 0000 0000 0000 0000
		//       1   11
		msg[0] = 0 | START_BIT | OP_OTHER | EW_EN;
	else
		msg[0] = 0 | START_BIT | OP_OTHER | EW_DS;

	SpiLock q(spi);
	q.Lock(-1);
	spi->SingleTransfer(1, msg, cs, false, ctar);
	q.UnLock();
}


void AT93C66::EraseAll(void)
{
	uint16 msg[1];

	msg[0] = 0 | START_BIT | OP_OTHER | ER_AL;

	SpiLock q(spi);
	q.Lock(-1);
	spi->SingleTransfer(1, msg, cs, false, ctar);
	q.UnLock();
}

void AT93C66::Erase(uint16 address)
{
	uint16 msg[1];
	msg[0] = 0 | START_BIT | OP_ERASE | (address & AT93C66ADDRESS_MASK);
	SpiLock q(spi);
	q.Lock(-1);
	spi->SingleTransfer(1, msg, cs, false, ctar);
	q.UnLock();
}

void AT93C66::WriteAll(uint16 data)
{
	uint16 msg[2];

	msg[0] = 0 | START_BIT | OP_OTHER | WR_AL;
	msg[1] = data;

	SpiLock q(spi);
	q.Lock(-1);
	spi->SingleTransfer(1, msg, cs, false, ctar);
	q.UnLock();
}
