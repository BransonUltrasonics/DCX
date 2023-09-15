/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/BDM08.cpp_v   1.5   21 Aug 2015 05:48:44   RJamloki  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2009        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/BDM08.cpp_v  $
 * 
 *    Rev 1.5   21 Aug 2015 05:48:44   RJamloki
 * Fixes for coverity CID 11189 and 11239
 * 
 *    Rev 1.4   24 Jul 2015 08:33:12   sesharma
 * Added definition of board dependent member functions of BDM08 class.
 * 
 *    Rev 1.3   02 Jul 2015 09:45:02   RJamloki
 * Added revision header as per review comment.
*/

#include "BDM08.h"
#include "CyGOS.h"
#include "CpuDefs.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "ArchDefs.h"
#include "RTOS.h"
#include "CPU.h"
#include "portable.h"
#include "GLOBALS.h"
#define printf
#define pprintf
// This fixes the CDT highlighter without messing up the compile process...
//#ifndef CYGOS_USE_DEBUGNEW
//#define WDT_EN
#define WDT_EN1
//#endif

//LOOP count and exit Status
UINT32 LoopCount = 0;
UINT8  BDMExitFlag  = FALSE;



/*  void BDM08::SetBkgdInSC1054(void)
 *
 *  Purpose:
 *    constructor of class BDM08.
 *
 *  Entry Condition:
 *    None
 *
 *  Exit Condition:
 *    None.
*/
BDM08::BDM08()
{
	//diagnostics = true;/*false;*/
	InitBDMPins();
	diagnostics = false;
	SetBkgdOut();
	SetBkgd(1);
	SetReset(1);
	//coverity CID: 11189
	cycleCount = 0;
}



/*  void BDM08::SetBkgdInSC1054(void)
 *
 *  Purpose:
 *    destructor of class BDM08.
 *
 *  Entry Condition:
 *    None.
 *
 *  Exit Condition:
 *    None.
*/
BDM08::~BDM08()
{
}



void BDM08::Reset10usBkgd(void)
{
	////SR sr = EnterCritical();
	for (int j = 0; j < 12; j++)
		SetReset(1);
	for (int i = 0; i < 125; i++)
	{
		SetReset(0);
		if (i == 10)
			SetBkgd(0);
	}
	SetReset(1);
//	ExitCritical(sr);
	CPU::DelayMs(10);
	SetBkgd(1);
	CPU::DelayMs(10);
	pprintf("(        1us) Reset  1\n");
	pprintf("(       11us) Reset  0\n");
	CPU::DelayMs(10);
}

#if Platform1A
inline int FastReadBkgd()
{
	return Board::cs4->inputs8_15 & 0x80;
}
#endif

/**
 * Sync: Reset the target CPU and pulse BDM low for 10ms. This will cause
 * target CPU to pulse BDM low as soon as it is set to high by the host CPU.
 * The low pulse length from the target is 128 BDM cycles long.
 * For Platform1A board using one LCD board BDM pulse was 124-128 counts long,
 * and over 98% of values were 127 or 128 counts.
 */
void BDM08::SyncCycle(int WaitCount)
{
	WaitCount = 0;
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
	diagnostics = true;
	SetBkgdOut();
	SetBkgd(0);
	printf("S");
	CPU::DelayMs(10);

	SetBkgd(1);
	SetBkgdIn();
	BDMExitFlag = FALSE;
	LoopCount = 0;
	while ((GetBkgd() != 0) && (BDMExitFlag == FALSE))
	{
		LoopCount++;
		#ifdef WDT_EN
		if((LoopCount & 0xF) == 0xF)
			CyGOS::KickWatchDog();
		#endif
		if(LoopCount >= LOOP_MAX)	BDMExitFlag = TRUE;
	}
	cycleCount = 0;
	if(BDMExitFlag == FALSE) {
		while (GetBkgd() == 0){
			cycleCount++;
			#ifdef WDT_EN
			if((cycleCount & 0xF) == 0xF)
				CyGOS::KickWatchDog();
			#endif
		}
	//	ExitCritical(sr);
		if (diagnostics){
			printf("Sync: %d\n", cycleCount);
		}
		if(cycleCount > MAX_COUNT)
		{
			while ((GetBkgd() != 0) && (BDMExitFlag == FALSE))
			{
				LoopCount++;
				#ifdef WDT_EN
				if((LoopCount & 0xF) == 0xF)
					CyGOS::KickWatchDog();
				#endif
				if(LoopCount >= LOOP_MAX)	BDMExitFlag = TRUE;
			}
			if(BDMExitFlag == FALSE) {
				while (GetBkgd() == 0){
					cycleCount++;
					#ifdef WDT_EN
					if((cycleCount & 0xF) == 0xF)
						CyGOS::KickWatchDog();
					#endif
				}
	#ifdef WDT_EN1
		CyGOS::KickWatchDog();
	#endif

			//	ExitCritical(sr);
				if (diagnostics){
					printf("Sync: %d\n", cycleCount);
				}
			}
		}
	}
	SetBkgd(1);
	SetBkgdOut();
	CPU::DelayMs(5);
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
}

void BDM08::Sync(void)
{
	char Count = 0;
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif

	SyncCycle(50000); //something like 2ms
/*	while((BDMExitFlag == TRUE) && Count++ <10){
		CPU::DelayMs(100);
		ReadSCR();
		ReadSCR();
		ReadSCR();
		printf("\n %d Try to Sync\n",Count+1);
		SyncCycle(LOOP_MAX); //something like 8ms
		CPU::DelayMs(50);
		}
*/
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif

	if(BDMExitFlag == TRUE)
		{
		    for(Count = 0; Count < 20; Count++){
#ifdef WDT_EN
		    	CyGOS::KickWatchDog();
#endif
		    	CPU::DelayMs(50);
		    }
			ReadSCR();
			ReadSCR();
			ReadSCR();
			SyncCycle(LOOP2_MAX);
			if(BDMExitFlag == TRUE) printf("\n***************SYNC FAILED******************\n");
		}

}

void BDM08::Transfer(uint8 * send, int sendLength, uint8 * recv, int recvLength)
{
	//CriticalSection cs;
	while (sendLength--)
	{
		Send(*send++);
	}
	int rc = (cycleCount + 4) / 8; // Counter for total reads per bit
	int counter = 0;
	while (GetBkgd() != 0 && counter < rc){
#ifdef WDT_EN
		if((counter & 0xF) == 0xF)
			CyGOS::KickWatchDog();
#endif
		counter++;
	}
	Recv(recv, recvLength);
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif

}

void BDM08::Recv(uint8 * recv, int recvLength)
{
	int tx = (cycleCount + 40) / 80; // Counter for zeroes when receiving
	int rc = (cycleCount + 4) / 8; // Counter for total reads per bit
	int bl = (cycleCount + 10) / 20; // Bit length, threshold between 0 and 1
	ASSERT(recvLength <= 2);
	int data[16];
	//SR sr = EnterCritical();
	int bits = 8 * recvLength;
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif

	for (int bit = 0; bit < bits; bit++)
	{
#ifdef WDT_EN
		if((bit & 0xF) == 0xF)
			CyGOS::KickWatchDog();
#endif
		SetBkgd(1);
		SetBkgdOut();
		for (int t = tx; t > 0; t--){
#ifdef WDT_EN
			if((t & 0xF) == 0xF)
				CyGOS::KickWatchDog();
#endif
			SetBkgd(0);
		}
		SetBkgdIn();
		SetBkgd(1);
		int counter = 0;
		while (GetBkgd() == 0){
#ifdef WDT_EN
			if((counter & 0xF) == 0xF)
				CyGOS::KickWatchDog();
#endif
			counter++;
		}
		data[bit] = counter;
		while (GetBkgd() != 0 && counter < rc){
#ifdef WDT_EN
			if((counter & 0xF) == 0xF)
				CyGOS::KickWatchDog();
#endif
			counter++;
		}
	}
	SetBkgd(1);
	SetBkgdOut();
	//ExitCritical(sr);
	for (int bit = 0; bit < bits; bit++)
	{
#ifdef WDT_EN
		if((bit & 0xF) == 0xF)
			CyGOS::KickWatchDog();
#endif
		int index = bit / 8;
		if ((bit & 7) == 0)
			recv[index] = 0;
		recv[index] <<= 1;
		if (data[bit] < bl)
			recv[index] |= 1;
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
}

#if Platform1A

void BDM08::Send(int byte)
{
	//SR sr = EnterCritical();
	SetBkgd(1);
	SetBkgdOut();
	for (int i = 0; i < 8; i++)
	{
		if (byte & 0x80)
		{
			// send high bit, 4 pulses low, 12 pulses high
			// at 220ns per pulse total period should be 3.6us, with
			// low 0.9us and high 2.7us
			SetBkgd(0);// 0.525	0.525
			SetBkgd(1);// 0.525	1.050
			SetBkgd(1);// 0.525	1.575
			SetBkgd(1);// 0.525	2.100
			SetBkgd(1);// 0.525	2.625
			SetBkgd(1);// 0.525	3.150
			SetBkgd(1);// 0.525	3.675
		}
		else
		{
			// send low bit, 13 pulses low, 3 pulses high
			// at 220ns per pulse total period should be 3.6us, with
			// low 2.9us and high 0.7us
			SetBkgd(0);// 0.525	0.525
			SetBkgd(0);// 0.525	1.050
			SetBkgd(0);// 0.525	1.575
			SetBkgd(0);// 0.525	2.100
			SetBkgd(1);// 0.525	2.625
			SetBkgd(1);// 0.525	3.150
			SetBkgd(1);// 0.525	3.675
		}
//		SetBkgd(1); // 0.525	3.675
//		SetBkgd(1); // 0.525	3.675
//		SetBkgd(1); // 0.525	3.675
		byte <<= 1;
	}
//	SetBkgdIn();
	SetBkgd(1);// 0.525	3.675
	SetBkgd(1);// 0.525	3.675
	SetBkgd(1);// 0.525	3.675
	SetBkgd(1);// 0.525	3.675
	SetBkgd(1);// 0.525	3.675
	SetBkgd(1);// 0.525	3.675
	//ExitCritical(sr);
}

#endif


void BDM08::Send(int byte)
{
	int tx00 = cycleCount / 13; // Counter for zeroes when sending '0'
	int tx01 = cycleCount / 20; // Counter for ones when sending '0'
	int tx10 = cycleCount / 40; // Counter for zeroes when sending '1'
	int tx11 = cycleCount / 10; // Counter for ones when sending '1'
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif

	//SR sr = EnterCritical();
	SetBkgd(1);
	SetBkgdOut();
	for (int i = 0; i < 8; i++)
	{
		if (byte & 0x80)
		{
			for (int t0 = tx10; t0 > 0; t0--){
#ifdef WDT_EN
				if((t0 & 0xF) == 0xF){
					CyGOS::KickWatchDog();
				}
#endif
				SetBkgd(0);
			}
			for (int t1 = tx11; t1 > 0; t1--){
#ifdef WDT_EN
				if((t1 & 0xF) == 0xF){
					CyGOS::KickWatchDog();
				}
#endif
				SetBkgd(1);
			}
		}
		else
		{
			for (int t0 = tx00; t0 > 0; t0--){
#ifdef WDT_EN
				if((t0 & 0xF) == 0xF){
					CyGOS::KickWatchDog();
				}
#endif
				SetBkgd(0);
			}
			for (int t1 = tx01; t1 > 0; t1--){
#ifdef WDT_EN
				if((t1 & 0xF) == 0xF){
					CyGOS::KickWatchDog();
				}
#endif
				SetBkgd(1);
			}
		}
		byte <<= 1;
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif

	//ExitCritical(sr);
}


int BDM08::ReadPC(void)
{
	uint8 send[1];
	send[0] = 0x6B;
	uint8 recv[2];
	Transfer(send, 1, recv, 2);
	int pc = (recv[0] << 8) | recv[1];
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Read PC %04X\n", pc);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
	return pc;
}

void BDM08::WritePC(int pc)
{
	uint8 send[3];
	send[0] = 0x4B;
	send[1] = pc >> 8;
	send[2] = pc;
	Transfer(send, 3, 0, 0);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Write PC %04X\n", pc);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
}

int BDM08::ReadSP(void)
{
	uint8 send[1];
	send[0] = 0x6F;
	uint8 recv[2];
	Transfer(send, 1, recv, 2);
	int sp = (recv[0] << 8) | recv[1];
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Read SP %04X\n", sp);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
	return sp;
}

void BDM08::WriteSP(int sp)
{
	uint8 send[3];
	send[0] = 0x4F;
	send[1] = sp >> 8;
	send[2] = sp;
	Transfer(send, 3, 0, 0);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Write SP %04X\n", sp);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
}

int BDM08::ReadCCR(void)
{
	uint8 send[1];
	send[0] = 0x69;
	uint8 recv[1];
	Transfer(send, 1, recv, 1);
	int ccr = recv[0];
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Read CCR %02X\n", ccr);
	}
	return ccr;
	//code removed for coverity CID:11239


}

void BDM08::WriteCCR(int ccr)
{
	uint8 send[2];
	send[0] = 0x49;
	send[1] = ccr;
	Transfer(send, 2, 0, 0);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Write CCR %02X\n", ccr);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
}

int BDM08::ReadSCR(void)
{
	uint8 send[1];
	send[0] = 0xE4;
	uint8 recv[1];
	Transfer(send, 1, recv, 1);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Read SCR %02X\n", recv[0]);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
	return recv[0];
}

void BDM08::WriteSCR(int value)
{
	uint8 send[2];
	send[0] = 0xC4;
	send[1] = value;
	Transfer(send, 2, 0, 0);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Write SCR %02X\n", value);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif

}

int BDM08::ReadA(void)
{
	uint8 send[1];
	send[0] = 0x68;
	uint8 recv[1];
	Transfer(send, 1, recv, 1);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Read A %02X\n", recv[0]);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
	return recv[0];
}

void BDM08::WriteA(int a)
{
	uint8 send[2];
	send[0] = 0x48;
	send[1] = a;
	Transfer(send, 2, 0, 0);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Write A %02X\n", a);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif

}

int BDM08::ReadHX(void)
{
	uint8 send[1];
	send[0] = 0x6C;
	uint8 recv[2];
	Transfer(send, 1, recv, 2);
	int hx = (recv[0] << 8) | recv[1];
	if (diagnostics){
		pprintf("Read HX %04X\n", hx);
	}
	CPU::DelayMs(1);
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
	return hx;
}

void BDM08::WriteHX(int hx)
{
	uint8 send[3];
	send[0] = 0x4C;
	send[1] = hx >> 8;
	send[2] = hx;
	Transfer(send, 3, 0, 0);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Write HX %04X\n", hx);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
}

void BDM08::Go(void)
{
	uint8 send[1];
	send[0] = 0x08;
	Transfer(send, 1, 0, 0);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Go\n");
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
}

void BDM08::Background(void)
{
	uint8 send[1];
	send[0] = 0x90;
	Transfer(send, 1, 0, 0);
	CPU::DelayMs(1);
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
}

int BDM08::ReadBkpt(void)
{
	uint8 send[1];
	send[0] = 0xE2;
	uint8 recv[2];
	Transfer(send, 1, recv, 2);
	CPU::DelayMs(1);
	int b = (recv[0] << 8) | recv[1];
	if (diagnostics){
		pprintf("Read BKPT %04X\n", b);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
	return b;
}

void BDM08::WriteBkpt(int value)
{
	uint8 send[3];
	send[0] = 0xC2;
	send[1] = value >> 8;
	send[2] = value;
	Transfer(send, 3, 0, 0);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Write BKPT %04X\n", value);
	}
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
}

void BDM08::Reset(void)
{
	SetReset(1);
	CPU::DelayMs(6);
	SetReset(0);
	SetReset(0);
	SetReset(0);
	SetReset(0);
	SetBkgdOut();
	SetBkgd(0);
	CPU::DelayMs(6);
	SetReset(1);
	CPU::DelayMs(6);
	SetBkgd(1);
	CPU::DelayMs(6);
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
}

void BDM08::ReadByte(int address, uint8 * data, int length)
{
	while (length--)
	{
		uint8 send[3];
		send[0] = 0xE0;
		send[1] = address >> 8;
		send[2] = address;
		uint8 recv[1];
		Transfer(send, 3, recv, 1);
		CPU::DelayMs(1);
		if (diagnostics){
			pprintf("Read byte [%04X] data %02X\n", address, recv[0]);
		}
		if (data)
			*data++ = recv[0];
		address++;
#ifdef WDT_EN
		if((length & 0xF) == 0xF)
			CyGOS::KickWatchDog();
#endif
	}
}

int BDM08::ReadByte(int address)
{
	uint8 send[3];
	send[0] = 0xE0;
	send[1] = address >> 8;
	send[2] = address;
	uint8 recv[1];
	Transfer(send, 3, recv, 1);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Read byte [%04X] data %02X\n", address, recv[0]);
	}

#ifdef WDT_EN
	CyGOS::KickWatchDog();
#endif
	return recv[0];
}

void BDM08::WriteByte(int address, uint8 * data, int length)
{
	while (length--)
	{
		uint8 send[4];
		send[0] = 0xC0;
		send[1] = address >> 8;
		send[2] = address;
		send[3] = *data++;
		Transfer(send, 4, 0, 0);
		CPU::DelayMs(1);
		if (diagnostics){
			pprintf("Write byte [%04X] data %02X\n", address, send[3]);
		}
#ifdef WDT_EN
		if((length & 0xF) == 0xF)
			CyGOS::KickWatchDog();
#endif
		address++;
	}
}

void BDM08::WriteByte(int address, int data)
{
	uint8 send[4];
	send[0] = 0xC0;
	send[1] = address >> 8;
	send[2] = address;
	send[3] = data;
	Transfer(send, 4, 0, 0);
	CPU::DelayMs(1);
	if (diagnostics){
		pprintf("Write byte [%04X] data %02X\n", address, data);
	}
#ifdef WDT_EN
	CyGOS::KickWatchDog();
#endif
}

void BDM08::ReadNext(int address, uint8 * data, int length)
{
	int hx = ReadHX();
	WriteHX(address - 1);
	while (length--)
	{
		uint8 send[1];
		send[0] = 0x70;
		uint8 recv[1];
		Transfer(send, 1, recv, 1);
		CPU::DelayMs(1);
		if (diagnostics){
			pprintf("Read [+HX] data %02X\n", recv[0]);
		}
		if (data)
			*data++ = recv[0];
#ifdef WDT_EN
		if((length & 0xF) == 0xF)
			CyGOS::KickWatchDog();
#endif
	}
	WriteHX(hx);
}

void BDM08::WriteNext(int address, uint8 data)
{
	WriteNext(address, &data, 1);
}

void BDM08::WriteNext(int address, uint8 * data, int length)
{
	int hx = ReadHX();
	WriteHX(address - 1);
	while (length--)
	{
		uint8 send[2];
		send[0] = 0x50;
		send[1] = *data++;
		Transfer(send, 2, 0, 0);
		CPU::DelayMs(1);
		if (diagnostics){
			pprintf("Write [+HX] data %02X\n", send[1]);
		}
#ifdef WDT_EN
		if((length & 0xF) == 0xF)
			CyGOS::KickWatchDog();
#endif
	}
	WriteHX(hx);
}

void BDM08::VerifyHXBlank(int address, int length)
{
	//diagnostics = true;
	int hx = ReadHX();
	WriteHX(address - 1);
	while (length--)
	{
		uint8 send[1];
		send[0] = 0x70;
		uint8 recv[1];
		Transfer(send, 1, recv, 1);
		CPU::DelayMs(1);
		if (diagnostics){
			pprintf("Read [+HX] at %04X data %02X\n", address, recv[0]);
		}
/*		if (recv[0] != 0xFF){
			printf("Verify failed, address %04X data %02X\n", address, recv[0]);
		}*/
#ifdef WDT_EN
		if((length & 0xF) == 0xF)
			CyGOS::KickWatchDog();
#endif
		address++;
	}
	WriteHX(hx);
}

void BDM08::PrepareProgram(void)
{
	static uint8 program[] =
	{ 0xA6, 0x20, 0xB7, 0x67, 0xB6, 0x64, 0xB7, 0x68, 0xB6, 0x65, 0xB7, 0x69,
			0xA6, 0xFF, 0xC7, 0x18, 0x24, 0xA6, 0x93, 0xB0, 0x63, 0xC7, 0x04, 0xBC, 0xC7, 0x04, 0xE4, 0xA6,
			0x00, 0xB2, 0x62, 0xC7, 0x04, 0xBB, 0xC7, 0x04, 0xE3, 0x55, 0x62,
			0xD6, 0xFF, 0xFF, 0xB7, 0x66, 0xBD, 0x6A, 0xAF, 0x01, 0x8B, 0x89,
			0x55, 0x64, 0xAF, 0xFF, 0x35, 0x64, 0x65, 0x00, 0x00, 0x88, 0x8A,
			0x27, 0x04, 0xBD, 0x89, 0x20, 0xE4, 0xB6, 0x68, 0xB7, 0x64, 0xB6, 0x69, 0xB7, 0x65, 0xBD,
			0x89, 0x55, 0x62, 0xD6, 0xFF, 0xFF, 0xF1, 0x26, 0x15, 0xAF, 0x01,
			0x8B, 0x89, 0x55, 0x64, 0xAF, 0xFF, 0x35, 0x64, 0x65, 0x00, 0x00,
			0x88, 0x8A, 0x26, 0xE9, 0x45, 0x00, 0x00, 0x82, 0x82\
	};
	pprintf("\nPrepare\n");
	WriteNext(0x0493, program, sizeof(program));
	ReadSCR();
//	ReadByte(0x04F6);
//	ReadByte(0x04F7);
//	WriteByte(0x04F6, 0x82);
//	WriteByte(0x04F7, 0x00);
//	Sync();
//	ReadSCR();
}

void BDM08::ProgramFlash(int address, uint8 * data, int length)
{
	CyGOS::KickWatchDog();
	// 008A is RAM address for up to 1K of data to be programmed
	WriteNext(0x0093, data, length);
	WriteByte(0x0060, 0x00);
	WriteByte(0x0061, 0x93);
	// This is the address in the flash to be programmed
	WriteByte(0x0062, address >> 8);
	WriteByte(0x0063, address >> 0);
	// This is the length of each fragment to be programmed
	WriteByte(0x0064, length >> 8);
	WriteByte(0x0065, length >> 0);
	WritePC(0x0493);
	WriteSP(0x081E);
	WriteCCR(0x6A);
	WriteA(0xFF);
	WriteHX(0x0000);
	Go();
	BDMExitFlag = TRUE;
//	UINT8 LoopFlag = FALSE;
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
	for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
	{
		CPU::DelayMs(10);
		int scr = ReadSCR();
		if (scr == 0xC8) {
			BDMExitFlag = FALSE;
			break;
		}
#ifdef WDT_EN
		if((LoopCount & 0xF) == 0xF)
			 CyGOS::KickWatchDog();
#endif
	/*	if((scr == 0x00) && (LoopFlag == FALSE)){
			int bkpt = ReadBkpt();
			WriteBkpt(0x1234);
			ReadBkpt();
			WriteBkpt(bkpt);
			LoopFlag = TRUE;
		}*/
	}
	Expect(ReadPC(), 0x04FD);
	Expect(ReadSP(), 0x081E);
	Expect(ReadCCR(), 0x6A);
	Expect(ReadA(), data[length - 1]);
	Expect(ReadHX(), 0x0000);
	Expect(ReadByte(0x1821), 0xC2);
	CyGOS::KickWatchDog();
}

void BDM08::ProgramFlashByte(int address, int data)
{
	uint8 byte[1];
	byte[0] = data;
	ProgramFlash(address, byte, 1);
}

void BDM08::ProgramFlashWord(int address, int data)
{
	uint8 byte[2];
	byte[0] = data >> 8;
	byte[1] = data >> 0;
	ProgramFlash(address, byte, 2);
}

void BDM08::Expect(int read, int expect)
{
	if (read != expect){
		pprintf("Read %02X, expected %02X\n", read, expect);
	}
}

void BDM08::PrepareCalculateChecksum()
{
	static uint8 codeSomething[] =
	{ 0x87, 0x8B, 0x89, 0xF6, 0x45, 0x00, 0xB7, 0x87, 0xEB, 0x01, 0xE7, 0x01,
			0x4F, 0xF9, 0xF7, 0x86, 0xAE, 0x08, 0x9E, 0xE8, 0x03, 0x48, 0x24,
			0x02, 0xA8, 0x07, 0x5A, 0x26, 0xF8, 0x9E, 0xE7, 0x03, 0x88, 0x8A,
			0x65, 0xFF, 0xFF, 0xAF, 0x01, 0x26, 0xD8, 0x86, 0x82, 0x9D, 0x9D,
			0x00, 0x00, 0x00 };
	static uint8 verifyBuffer[sizeof(codeSomething)];
	WriteByte(0x008A, codeSomething, sizeof(codeSomething));
	ReadByte(0x008A, verifyBuffer, sizeof(codeSomething));
#ifdef WDT_EN1
	CyGOS::KickWatchDog();
#endif
	if (memcmp(codeSomething, verifyBuffer, sizeof(codeSomething)) != 0){
		pprintf("Verify Code for Something failed\n");
	}
}

int BDM08::CalculateChecksum(int address, int length)
{
	int end = address + length - 1;
	WriteByte(0x00AD, end >> 8);
	WriteByte(0x00AE, end >> 0);
	// Initial value
	WriteByte(0x00B7, 0x00);
	WriteByte(0x00B8, 0x00);
	WritePC(0x008A);
	WriteSP(0x081E);
	WriteCCR(0x6A);
	WriteA(0x00);
	WriteHX(address);
	Go();
	BDMExitFlag = TRUE;
	for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
	{
		CPU::DelayMs(10);
		int scr = ReadSCR();
		if (scr == 0xC8) {
			BDMExitFlag = FALSE;
			break;
		}
	}
	Expect(ReadPC(), 0x00B5);
	Expect(ReadSP(), 0x081E);
	Expect(ReadCCR(), 0x6A);
	ReadA(); // don't know what to expect
	Expect(ReadHX(), address + length);
	int b1 = ReadByte(0x00B7);
	int b2 = ReadByte(0x00B8);
	return (b1 << 8) | b2;
}
uint8 WriteData, ReadData;
int Address;
void BDM08::VerifyHX(int address, uint8 * data, int length)
{
	int hx = ReadHX();
	int indx = 0;
	WriteHX(address - 1);
	while (length--)
	{
		uint8 send[1];
		send[0] = 0x70;
		uint8 recv[1];
		Transfer(send, 1, recv, 1);
		CPU::DelayMs(1);
		if (diagnostics){
			pprintf("Read [+HX] data %02X\n", recv[0]);
		}
		if (recv[0] != *data++){
			pprintf("Verify failed, address %04X data %02X\n", address, recv[0]);
			BDMExitFlag = TRUE;
			length = 0;
			ReadData = recv[0];
			WriteData = *(data - 1);
			Address = address;
		}
		address++;
#ifdef WDT_EN
		if((length & 0xF) == 0xF)
			CyGOS::KickWatchDog();
#endif
	}
	WriteHX(hx);
}



/*  void BDM08::InitBDMPinsWC991(void)
 *
 *  Purpose:
 *    This function configures CPU pins required for LCD programming through
 *    BDM as general purpose output.
 *		It is called from BDM08 object’s constructor
 *
 *  Entry Condition:
 * 	LCD Programming initiated.
 *
 *  Exit Condition:
 * 	CPU pins(H3 and G1) connected to Background Debug and reset lines of
 * 	LCD processor configured as general purpose output.
*/
void BDM08::InitBDMPinsWC991(void)
{
   //PTIMER1 and PTIMER0 pins configured as normal GPIO
   MCF_PAD_PAR_TIMER &=
         ~(MCF_PAD_PAR_TIMER_PAR_T0IN(MCF_PAD_PAR_TIMER_PAR_T0IN_T0IN)
               | MCF_PAD_PAR_TIMER_PAR_T1IN(MCF_PAD_PAR_TIMER_PAR_T1IN_T1IN));
   MCF_GPIO_PDDR_TIMER |= (MCF_GPIO_PDDR_TIMER_PDDR_TIMER1
         | MCF_GPIO_PDDR_TIMER_PDDR_TIMER0);
}



/*  void BDM08::SetResetWC991(int value)
 *
 *  Purpose:
 *    This function drives G1 pin connected to reset line of LCD as per the passed argument
 *    It is called from BDM08::Reset () function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming. Value contains the logic to be driven on reset line.
 *
 *  Exit Condition:
 *		LCD_RESET line gets driven to required logic.
*/
void BDM08::SetResetWC991(int value)
{
   if (value == 0)
      MCF_GPIO_PODR_TIMER &= ~MCF_GPIO_PODR_TIMER_PODR_TIMER0;		// Drive Low
   else
      MCF_GPIO_PODR_TIMER |= MCF_GPIO_PODR_TIMER_PODR_TIMER0;		// Drive High
}



/*  void BDM08::SetBkgdWC991(int value)
 *
 *  Purpose:
 *    This function drives H3 pin connected to back ground debug line of LCD
 *    as per the passed argument.
 * 	It is called from BDM08::Reset () function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming to set the target processor on LCD board in back ground debug mode.
 * 	Value contains the logic to be driven on reset line.
 *
 *  Exit Condition:
 * 	LCD_BKGD line gets driven to required logic.
*/
void BDM08::SetBkgdWC991(int value)
{
   if (value == 0)
      MCF_GPIO_PODR_TIMER &= ~MCF_GPIO_PODR_TIMER_PODR_TIMER1;		// Drive Low
   else
      MCF_GPIO_PODR_TIMER |= MCF_GPIO_PODR_TIMER_PODR_TIMER1;		// Drive High
}



/*  int BDM08::GetBkgdWC991(void)
 *
 *  Purpose:
 *    To get the status of LCD_BKGD line
 *    It is called from BDM08::Reset () function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming.
 *
 *  Exit Condition:
 * 	Status of H3 pin is returned.
*/
int BDM08::GetBkgdWC991(void)
{
	return MCF_GPIO_PPDSDR_TIMER & MCF_GPIO_PPDSDR_TIMER_PPDSDR_TIMER1;	
}



/*  void SetBkgdOutWC991(void)
 *
 *  Purpose:
 *    To configure the H3 pin connected to LCD_BKGD line as output.
 *    It is called from BDM08::Send () function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming.
 *
 *  Exit Condition:
 * 	H3 pin is configured as general purpose output.
*/
void BDM08::SetBkgdOutWC991(void)
{
	MCF_GPIO_PDDR_TIMER |= MCF_GPIO_PDDR_TIMER_PDDR_TIMER1;	
}



/*  void BDM08::SetBkgdInWC991(void)
 *
 *  Purpose:
 *    To configure the H3 pin connected to LCD_BKGD line as output..
 *    It is called from BDM08:: SyncCycle() function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming.
 *
 *  Exit Condition:
 * 	H3 pin is configured as general purpose input.
*/
void BDM08::SetBkgdInWC991(void)
{
	MCF_GPIO_PDDR_TIMER &= ~MCF_GPIO_PDDR_TIMER_PDDR_TIMER1;
}



/*  void BDM08::InitBDMPinsWC1169(void)
 *
 *  Purpose:
 *    This function configures CPU pins required for LCD programming through BDM as general purpose output.
 *    It is called from BDM08 object’s constructor
 *
 *  Entry Condition:
 * 	LCD Programming initiated.
 *
 *  Exit Condition:
 * 	CPU pins(H3 nad G1) connected to Background Debug and reset lines of
 * 	LCD processor configured as general purpose output.
*/
void BDM08::InitBDMPinsWC1169(void)
{
   //PTIMER1 and PTIMER0 pins configured as normal GPIO
   MCF_PAD_PAR_TIMER &=
         ~(MCF_PAD_PAR_TIMER_PAR_T0IN(MCF_PAD_PAR_TIMER_PAR_T0IN_T0IN)
               | MCF_PAD_PAR_TIMER_PAR_T1IN(MCF_PAD_PAR_TIMER_PAR_T1IN_T1IN));
   MCF_GPIO_PDDR_TIMER |= (MCF_GPIO_PDDR_TIMER_PDDR_TIMER1
         | MCF_GPIO_PDDR_TIMER_PDDR_TIMER0);
}



/*  void SetResetWC1169(int value)
 *
 *  Purpose:
 *    This function drives G1 pin connected to reset line of LCD as
 *    per the passed argument.
 *    It is called from BDM08::Reset () function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming. Value contains the logic to be driven on reset line.
 *
 *  Exit Condition:
 *		LCD_RESET line gets driven to required logic.
*/
void BDM08::SetResetWC1169(int value)
{
   if (value == 0)
      MCF_GPIO_PODR_TIMER &= ~MCF_GPIO_PODR_TIMER_PODR_TIMER0;   //Drive Low
   else
      MCF_GPIO_PODR_TIMER |= MCF_GPIO_PODR_TIMER_PODR_TIMER0;		//Drive High
}



/*  void BDM08::SetBkgdWC1169(int value)
 *
 *  Purpose:
 *    This function drives H3 pin connected to back ground debug line of LCD as per the passed argument.
 * 	It is called from BDM08::Reset () function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming to set the target processor on LCD board in back ground debug mode.
 * 	Value contains the logic to be driven on reset line.
 *
 *  Exit Condition:
 * 	LCD_BKGD line gets driven to required logic.
*/
void BDM08::SetBkgdWC1169(int value)
{
   if (value == 0)
      MCF_GPIO_PODR_TIMER &= ~MCF_GPIO_PODR_TIMER_PODR_TIMER1;		//Drive Low
   else
      MCF_GPIO_PODR_TIMER |= MCF_GPIO_PODR_TIMER_PODR_TIMER1;		//Drive High
}



/*  int BDM08::GetBkgdWC1169(void)
 *
 *  Purpose:
 *    To get the status of LCD_BKGD line.
 *    It is called from BDM08::Reset () function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming.
 *
 *  Exit Condition:
 * 	Status of H3 pin connected to LCD_BKGD line is returned.
*/
int BDM08::GetBkgdWC1169(void)
{
	return MCF_GPIO_PPDSDR_TIMER & MCF_GPIO_PPDSDR_TIMER_PPDSDR_TIMER1;
}



/*  void BDM08::SetBkgdOutWC1169(void)
 *
 *  Purpose:
 *    To configure the H3 pin connected to LCD_BKGD line as output.
 *    It is called from BDM08::Send () function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming.
 *
 *  Exit Condition:
 * 	H3 pin is configured as general purpose output.
*/
void BDM08::SetBkgdOutWC1169(void)
{
	MCF_GPIO_PDDR_TIMER |= MCF_GPIO_PDDR_TIMER_PDDR_TIMER1;
}



/*  void BDM08::SetBkgdInWC1169(void)
 *
 *  Purpose:
 *    To configure the H3 pin connected to LCD_BKGD line as input.
 *    It is called from BDM08:: SyncCycle() function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming.
 *
 *  Exit Condition:
 * 	H3 pin is configured as general purpose input.
*/
void BDM08::SetBkgdInWC1169(void)
{
	MCF_GPIO_PDDR_TIMER &= ~MCF_GPIO_PDDR_TIMER_PDDR_TIMER1;
}



/*  void BDM08::InitBDMPinsSC1054(void)
 *
 *  Purpose:
 *    This function configures CPU pins required for LCD programming through BDM as general purpose output.
 *    It is called from BDM08 object’s constructor
 *
 *  Entry Condition:
 * 	LCD Programming initiated.
 *
 *  Exit Condition:
 * 	CPU pins(Y16 and AA16) connected to Background Debug and reset lines of
 * 	LCD processor configured as general purpose output.
*/
void BDM08::InitBDMPinsSC1054(void)
{
   MCF_PAD_PAR_UART &= ~(MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS
         | MCF_PAD_PAR_UART_PAR_U0CTS_U0CTS);   // U0RTS and U0CTS pins configured as normal GPIO
   MCF_GPIO_PDDR_UART |= (MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS
         | MCF_PAD_PAR_UART_PAR_U0CTS_U0CTS);   // Drive High U0RTS and U0CTS pins
}



/*  void BDM08::SetResetSC1054(int value)
 *
 *  Purpose:
 *    This function drives AA16 pin connected to reset line of LCD as per the passed argument
 *    It is called from BDM08::Reset () function of BDM08.cpp.
 *  Entry Condition:
 * 	Called during LCD Programming. Value contains the logic to be driven on reset line.
 *
 *  Exit Condition:
 *		LCD_RESET line gets driven to required logic.
*/
void BDM08::SetResetSC1054(int value)
{
   if (value == 0)
      MCF_GPIO_PODR_UART &= ~MCF_PAD_PAR_UART_PAR_U0RTS;		//Drive Low
   else
      MCF_GPIO_PODR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS;		//Drive High
}



/*  void BDM08::SetBkgdSC1054(int value)
 *
 *  Purpose:
 *    This function drives Y16 pin connected to back ground debug line of LCD as per the passed argument.
 * 	It is called from BDM08::Reset () function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming to set the target processor on LCD board in back ground debug mode.
 * 	Value contains the logic to be driven on reset line.
 *
 *  Exit Condition:
 * 	LCD_BKGD line gets driven to required logic.
*/
void BDM08::SetBkgdSC1054(int value)
{
   if (value == 0)
      MCF_GPIO_PODR_UART &= ~MCF_PAD_PAR_UART_PAR_U0CTS;		//Drive Low
   else
      MCF_GPIO_PODR_UART |= MCF_PAD_PAR_UART_PAR_U0CTS;		//Drive High
}



/*  int BDM08::GetBkgdSC1054(void)
 *
 *  Purpose:
 *    To get the status of LCD_BKGD line.
 *    It is called from BDM08::Reset () function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming.
 *
 *  Exit Condition:
 * 	Status of Y16 pin is returned.
*/
int BDM08::GetBkgdSC1054(void)
{
	return MCF_GPIO_PPDSDR_UART & MCF_PAD_PAR_UART_PAR_U0CTS;
}



/*  void BDM08::SetBkgdOutSC1054(void)
 *
 *  Purpose:
 *    To configure the Y16 pin connected to LCD_BKGD line as output.
 *    It is called from BDM08::Send () function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming.
 *
 *  Exit Condition:
 * 	Y16 pin is configured as general purpose output.
*/
void BDM08::SetBkgdOutSC1054(void)
{
	MCF_GPIO_PDDR_UART |= MCF_PAD_PAR_UART_PAR_U0CTS;		//Drive High
}



/*  void BDM08::SetBkgdInSC1054(void)
 *
 *  Purpose:
 *    To configure the Y16 pin connected to LCD_BKGD line as input.
 *    It is called from BDM08:: SyncCycle() function of BDM08.cpp.
 *
 *  Entry Condition:
 * 	Called during LCD Programming.
 *
 *  Exit Condition:
 * 	Y16 pin is configured as general purpose input.
*/
void BDM08::SetBkgdInSC1054(void)
{
	MCF_GPIO_PDDR_UART &= ~MCF_GPIO_PDDR_UART_PDDR_UART3;
}


