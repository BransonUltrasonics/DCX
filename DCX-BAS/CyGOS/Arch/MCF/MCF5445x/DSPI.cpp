// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/DSPI.cpp_v   1.1   11 Mar 2013 02:39:22   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares DSPI driver for MCF54455 processor.

#include "DSPI.h"
#include "CpuDefs.h"
#include "CPU.h"
#include "Task.h"
#include "assert.h"
#include "stdio.h"

DSPI * DSPI::thisPtr;

static const uint32 BaudratePrescaler[] =
{ 2, 3, 5, 7 };
static const uint32 BaudrateScaler[] =
{ 2, 4, 6, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };

/**
 * 	DSPI constructor
 * 	@param instance : Only 1 module in 5445x series
 * 	@param irqlevel	: IRQ level
 * 	@param irqprio : IRQ priority
 */
DSPI::DSPI(int instance, int irqlevel, int irqprio)
{
	ASSERT(instance == 0);
	ASSERT(thisPtr == 0);
	spiflag = false;//if spi device set the flag to reverse of what is expected
	                 //for one time intitalizations
	thisPtr = this;
	reg = (DSPI_Regs *) &MCF_DSPI_DSPI_MCR;
	CPU::SetupHandler(InterruptHandler, intDSPI, irqlevel, irqprio, 256);
}

/**
 * 	DSPI Destructor
 */
DSPI::~DSPI()
{
	thisPtr = 0;
	// TODO: Install the Default Interrupt handler here as this object is no more
}

/**
 * 	Calculate Baudrate given the clock
 * 	@param clock : Fsys/2
 * 	@param baud : SCK frquency needed
 */
uint32 DSPI::GetBaudRate(uint32 clock, uint32 baud, bool pol, bool pha)
{
	uint32 pres, scaler, min, minpres=0, minscaler=0;
	int val;

	min = (uint32) -1;//assign 0xFFFFFFFF
	for (pres = 0; pres < 4; pres++)
	{
		for (scaler = 0; scaler < 16; scaler++)
		{
			val = BaudratePrescaler[pres] * BaudrateScaler[scaler] * baud
					- clock;
			if (val < 0)
				val = -val;
			if (min > (uint32)val)
			{
				min = val;
				minpres = pres;
				minscaler = scaler;
			}
		}
	}
	int timing = 0;
	// After the clock is computed, we need to also set PCS to SCK delays right
	if (((!pol) && (!pha)) || (pol && (!pha)))
	{
		// CPOL=any and CPHA=0, set PCS to SCK delay to half SCK period
		int sck = BaudratePrescaler[minpres] * BaudrateScaler[minscaler] / 2;
		// allowed range will be 40% to 60% of the full period, if
		// this fails, we'll get ASSERT and go from there...
		int sckmin = (sck * 4) / 10;
		int sckmax = (sck * 6) / 10;
		bool found = false;
		for (int cssck = 0; cssck < 16; cssck++)
		{
			for (int pcssck = 0; pcssck < 4; pcssck++)
			{
				int delay = (1 << cssck) * (pcssck * 2 + 1);
				if ((delay >= sckmin) && (delay <= sckmax))
				{
					timing = MCF_DSPI_DSPI_CTAR_CSSCK(cssck)
							| MCF_DSPI_DSPI_CTAR_PCSSCK(pcssck);
					found = true;
					break;
				}
			}
			if (found)
				break;
		}
		ASSERT(found);
	}
	else
	{
		printf("CPOL %d and CPHA %d not supported, verify timings!\n", pol, pha);
	}
	return timing | MCF_DSPI_DSPI_CTAR_PBR(minpres)
			| MCF_DSPI_DSPI_CTAR_BR(minscaler);
}

/**
 * 	Configure pins for SPI operation
 */
void DSPI::ConfigureSPIPins()
{
	// Enable SPI operation
	MCF_PAD_PAR_DSPI = 0 | MCF_PAD_PAR_DSPI_PAR_SOUT_SOUT
			| MCF_PAD_PAR_DSPI_PAR_SIN_SIN | MCF_PAD_PAR_DSPI_PAR_SCK_SCK
			| MCF_PAD_PAR_DSPI_PAR_PCS0_PCS0 | MCF_PAD_PAR_DSPI_PAR_PCS1_PCS1
			| MCF_PAD_PAR_DSPI_PAR_PCS2_PCS2 | MCF_PAD_PAR_DSPI_PAR_PCS5_PCS5;

	// Drive strength max
	MCF_PAD_DSCR_DSPI = MCF_PAD_DSCR_DSPI_DSE_SPI(3);
}

/**
 * 	Configure and Enable SPI device
 * 	@param phase : Clock phase high or low
 * 	@param bits : # of bits per transfer 4-16
 * 	@param clock : SCK baud rate
 * 	@param polarity : Clock polarity high or low
 * 	@param lsb : lsb bit first
 */
void DSPI::EnableSPI(bool phase, int bits, int clock, bool polarity, bool lsb)
{
	ASSERT((bits >= 4) && (bits <= 16));

	if(!spiflag)
	{
	// Configure pins for SPI operation
	ConfigureSPIPins();

	// Disable and clear SPI
	reg->MCR = MCF_DSPI_DSPI_MCR_HALT;
	for (volatile int i = 0; i < 10; i++)
	{
	}
	reg->MCR = MCF_DSPI_DSPI_MCR_HALT | MCF_DSPI_DSPI_MCR_CLR_TXF
			| MCF_DSPI_DSPI_MCR_CLR_RXF;

	// Set MCR registers now
	reg->MCR = 0 | MCF_DSPI_DSPI_MCR_MSTR // master mode
			//| MCF_DSPI_DSPI_MCR_CONT_SCKE // continuous Clock
			//| MCF_DSPI_DSPI_MCR_MTFE // modified transfer format
			//| MCF_DSPI_DSPI_MCR_PCSSE // CS strobe
			//| MCF_DSPI_DSPI_MCR_ROOE // Rx data after full fifo is shifted
			| MCF_DSPI_DSPI_MCR_PCSIS0 // Default CS0 is high
			| MCF_DSPI_DSPI_MCR_PCSIS1 // Default CS1 is high
			| MCF_DSPI_DSPI_MCR_PCSIS2 // Default CS2 is high
			| MCF_DSPI_DSPI_MCR_PCSIS3 // Default CS3 is high
			| MCF_DSPI_DSPI_MCR_PCSIS5 // Default CS5 is high
			//| MCF_DSPI_DSPI_MCR_SMPL_PT_0CLK // 0 system clocks b/w DSPI_SCK edge & DSPI_SIN sample
			| MCF_DSPI_DSPI_MCR_HALT; // do not enable transfers yet
	}

	// Set CTAR registers now
	int ctar = 0;

//	if((bits == nBits) && (clock == Speed) && (lsb== Lsb) && (polarity == Polarity) && (phase == Phase)
//			&& (spiflag == true))
//	{
//		return;
//	}
	if((clock != Speed) || (polarity != Polarity) || (phase != Phase) || !spiflag)
	{
		baud = GetBaudRate(__SYSTEM_CLOCK * 1000000, clock,
				polarity, phase); // sck frequency
	}

	ctar |= MCF_DSPI_DSPI_CTAR_FMSZ(bits - 1); // frame size
	if (polarity)
		ctar |= MCF_DSPI_DSPI_CTAR_CPOL; // clock polarity
	if (phase)
		ctar |= MCF_DSPI_DSPI_CTAR_CPHA; // clock phase
	if (lsb)
		ctar |= MCF_DSPI_DSPI_CTAR_LSBFE; // lsb first enable
	reg->CTAR[0] = ctar | baud;
	// clear all status bits
	reg->SR = -1;
	// disable all interrupts
	reg->RSER = 0;

	Speed = clock;
	nBits = bits;
	Lsb = lsb;
	Phase = phase;
	Polarity = polarity;
	spiflag = true;
}

unsigned char DSPI::ReadByte(unsigned char send, int chipSelect)
{
	unsigned char dummy = 0;
	CRASH("Not Implemented\n");
	// To avoid compiler warning of unused variable.
	ASSERT(send);
	ASSERT(chipSelect);
	return dummy;
}

/**
 * SingleTransfer: Transfer a single packet of data. Function assumes that QSPI
 * driver is acquired before the function is called. Data is transferred in
 * up to 16-bit quantities, depending on word length.
 * @param length Transfer length in words.
 * @param data Transfer buffer. At entry the buffer contains transmit data,
 * then the buffer is filled with receive data. Each element is 16 bits, lower
 * bits are used for transfer. Two 8-bit transfers will not be packed into one
 * 16-bit word in this buffer.
 * @param chipSelect Chip select pin value during the transfer.
 * @param cont If true CS returns to inactive state after the end of last
 * transfer, else CS stays active. All 4-16 bit word transfers within single
 * packet are performed with CS active between transfers.
 */
void DSPI::SingleTransfer(int length, uint16 * data, int chipSelect, bool cont)
{
	int cs = (chipSelect & 0xFF) << 16;

	ASSERT(length <= 16);

	// prepare to transfer data now
	for (int i = 0; i < length; i++)
	{
		// wait for atleast 1 empty spot in the fifo
		while (!(reg->SR & MCF_DSPI_DSPI_SR_TFFF))
			;
		int pushr = MCF_DSPI_DSPI_PUSHR_CTAS(0); // first CTAR register
		if (cont || (i < (length - 1)))
			pushr |= MCF_DSPI_DSPI_PUSHR_CONT; // continuous
		pushr |= cs; // CS to assert
		reg ->PUSHR = pushr | MCF_DSPI_DSPI_PUSHR_TXDATA(data[i]);
		for (volatile int j = 0; j < 5; j++)
			;
	}
	// clear Transfer Complete flag just in case
	reg->SR = MCF_DSPI_DSPI_SR_TCF;
	// enable spi module transfers
	reg->MCR &= ~MCF_DSPI_DSPI_MCR_HALT;
	// now wait for transfer complete
	int loop = 0;
	while (!(reg->SR & MCF_DSPI_DSPI_SR_TCF))
	{
		// just some extra safety so that we don't hang without messages
		// TODO this may be too low limit for slow long transfers
		loop++;
		ASSERT(loop < 1000);
	}
	// check for Rx fifo overflow conditions
	ASSERT(!(reg->SR & MCF_DSPI_DSPI_SR_RFOF));
	for (int i = 0; i < length; i++)
	{
		// read as long as rx fifo is not empty
		loop = 0;
		while (!(reg->SR & MCF_DSPI_DSPI_SR_RFDF))
		{
			// just some extra safety so that we don't hang without messages
			// here we provide delays between RX FIFO addesses, so limit is low
			loop++;
			ASSERT(loop < 1000);
		}
		data[i] = reg->POPR;
		// TODO maybe not needed as an operation, but it inserts a delay...
		reg->SR = MCF_DSPI_DSPI_SR_RFDF; // clear the flag now
	}
	// disable spi module transfers
	reg->MCR |= MCF_DSPI_DSPI_MCR_HALT;
}

/**
 * LongWordTransfer: Execute long interrupt-driven QSPI data transfer. Function
 * assumes that SPI is acquired. Current task is suspended while transfer is
 * in progress. All transfers are performed with continuous CS value, CS is
 * deactivated after all transfers are complete. All transfers are in up to
 * 16-bit words, source data is interpreted as an array of 4-16 bit words stored
 * in 16 bits per word, with MSB unused if transfer is shorter than 16 bits per
 * word.
 * @param length Total transfer length, may be more than what fits into QSPI at
 * one time.
 * @param data Transfer buffer, initially filled with transmit data, when
 * transfer is complete, buffer is filled with received data.
 * @param maxLength Maximum transfer length to use in one DSPI transfer.
 * @param chipSelect Chip select pin states for the transfer.
 */
void DSPI::LongWordTransfer(int length, uint8 * data, int maxLength,
		int chipSelect)
{
	ASSERT((length & 1) == 0);
	transferLength = length >> 1;
	transferData = (uint16 *) data;
	transferOnce = maxLength;
	transferCS = (chipSelect & 0xFF) << 16;
	// This many words must be transferred in first go
	length = transferLength;
	if (length > transferOnce)
		length = transferOnce;
	// Transfer data now
	for (int i = 0; i < length; i++)
	{
		ASSERT(reg->SR & MCF_DSPI_DSPI_SR_TFFF);
		int cont = MCF_DSPI_DSPI_PUSHR_CONT;
		if (i == (transferLength - 1))
			cont = 0;
		int tx = 0 | MCF_DSPI_DSPI_PUSHR_CTAS(0) // first CTAR register
				| cont | transferCS // CS to assert
				| MCF_DSPI_DSPI_PUSHR_TXDATA(transferData[i]);
		reg->PUSHR = tx;
	}
	CriticalSection s;
	// clear pending interrupt if any
	reg->SR |= MCF_DSPI_DSPI_SR_TCF;
	// and enable transfer interrupts
	reg->RSER |= MCF_DSPI_DSPI_RSER_TCF_RE;
	// enable spi module transfers
	reg->MCR &= ~MCF_DSPI_DSPI_MCR_HALT;
	// We are executing in context of owner task, so here we suspend the task
	// and interrupt handler will resume the task when everything is done.
	mutex.GetOwnerTask()->Suspend();
}

/**
 * 	GPIO mode for operation in bit bang mode
 */
void DSPI::EnableGPIO()
{
	CRASH("Not Implemented\n");
}

/**
 * 	GPIO mode for operation in bit-bang mode
 */
void DSPI::SetSignals(int clock, int output, int cs)
{
	CRASH("Not Implemented\n");
	// To avoid compiler warning of unused variable.
	clock = 0;
	output = 0;
	cs = 0;
}

/**
 * 	GPIO mode for operation in bit-bang mode
 */
int DSPI::ReadInput()
{
	CRASH("Not Implemented\n");
}

void DSPI::InterruptHandler(int level, void * frame)
{
	if (level == IntVector(intDSPI))
	{
		thisPtr->IntHandler();
		return;
	}
	else
	{
		CRASH("Bad DSPI Interrupt");
	}
	frame = 0;
}

void DSPI::IntHandler()
{
	// First, extract received data... Limit transfer amount to 16.
	int length = transferLength;
	if (length > transferOnce)
		length = transferOnce;
	// validate if we have enough buffer to hold all data received
	for (int i = 0; i < length; i++)
	{
		// wait for rx fifo not empty
		int loop = 0;
		while (!(reg->SR & MCF_DSPI_DSPI_SR_RFDF))
		{
			// just some extra safety so that we don't hang without messages
			// here we provide delays between RX FIFO addesses, so limit is low
			loop++;
			ASSERT(loop < 1000);
		}
		int rx = reg->POPR;
		*transferData++ = rx;
		transferLength--;
		// TODO maybe not needed as an operation, but it inserts a delay...
		reg->SR = MCF_DSPI_DSPI_SR_RFDF; // clear the flag now
	}
	// disable spi module transfers
	reg->MCR |= MCF_DSPI_DSPI_MCR_HALT;
	// Now check if anything is left to send...
	if (transferLength == 0)
	{
		// done, disable all interrupts
		reg->RSER = 0;
		reg->SR = -1;
		mutex.GetOwnerTask()->Resume();
	}
	else
	{
		length = transferLength;
		if (length > transferOnce)
			length = transferOnce;
		// Transfer data now
		for (int i = 0; i < length; i++)
		{
			ASSERT(reg->SR & MCF_DSPI_DSPI_SR_TFFF);
			int cont = MCF_DSPI_DSPI_PUSHR_CONT;
			if (i == (transferLength - 1))
				cont = 0;
			int tx = 0 | MCF_DSPI_DSPI_PUSHR_CTAS(0) // first CTAR register
					| cont | transferCS // CS to assert
					| MCF_DSPI_DSPI_PUSHR_TXDATA(transferData[i]);
			reg->PUSHR = tx;
		}
		reg->SR |= MCF_DSPI_DSPI_SR_TCF;
		// enable spi module transfers
		reg->MCR &= ~MCF_DSPI_DSPI_MCR_HALT;
	}
}
