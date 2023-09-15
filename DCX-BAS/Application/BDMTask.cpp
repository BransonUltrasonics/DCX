// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/BDMTask.cpp_v   1.9   Mar 06 2017 16:18:20   EGUAJARDO  $
//
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/BDMTask.cpp_v  $
 * 
 *    Rev 1.9   Mar 06 2017 16:18:20   EGUAJARDO
 * Applied coding standard.
*/

#include "CYGOS.h"
#include "CPU.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "Board.h"
#include "BDM08.h"
#include "DebugNew.h"
#include "Task.h"
#include "RTOS.h"
//#include "LCDCode.h"
#include "portable.h"
#include "BDMTask.h"

//#define printf
//#define pprintf

extern UINT16 LoopCount;
extern uint8 WriteData, ReadData;
extern int Address;


void LoopReset()
{
	BDM08 bdm;
	for (;;)
	{
		bdm.SetReset(0);
		bdm.SetReset(0);
		bdm.SetReset(0);
		bdm.SetReset(0);
		bdm.SetReset(1);
		bdm.SetReset(1);
		bdm.SetReset(1);
		bdm.SetReset(1);
	}
}

static int tx00, tx01, tx10, tx11;
void LoopBkgd()
{
	char buf[4];
	printf("\nTX 0 (0) [%d]: ", tx00);
	gets(buf);
	if (strlen(buf) > 0)
		tx00 = atoi(buf);
	printf("TX 0 (1) [%d]: ", tx01);
	gets(buf);
	if (strlen(buf) > 0)
		tx01 = atoi(buf);
	printf("TX 1 (0) [%d]: ", tx10);
	gets(buf);
	if (strlen(buf) > 0)
		tx10 = atoi(buf);
	printf("TX 1 (1) [%d]: ", tx11);
	gets(buf);
	if (strlen(buf) > 0)
		tx11 = atoi(buf);
	BDM08 bdm;
	bdm.SetBkgdOut();
	for (int i = 0; i < 1000000; i++)
	{
		// each 100 gives 8us of time
		for (int x00 = tx00; x00 > 0; x00--)
			bdm.SetBkgd(0);
		for (int x01 = tx01; x01 > 0; x01--)
			bdm.SetBkgd(1);
		for (int x10 = tx10; x10 > 0; x10--)
			bdm.SetBkgd(0);
		for (int x11 = tx11; x11 > 0; x11--)
			bdm.SetBkgd(1);
	}
	printf("Done.\n");
}

void LoopBkgdDir()
{
	BDM08 bdm;
	for (;;)
	{
		bdm.SetBkgd(0);
		bdm.SetBkgd(0);
		bdm.SetBkgd(0);
		bdm.SetBkgd(0);
		bdm.SetBkgd(1);
		bdm.SetBkgd(1);
		bdm.SetBkgd(1);
		bdm.SetBkgd(1);
		bdm.SetBkgdIn();
		bdm.SetBkgdIn();
		bdm.SetBkgdIn();
		bdm.SetBkgdIn();
		bdm.SetBkgdOut();
		bdm.SetBkgdOut();
		bdm.SetBkgdOut();
		bdm.SetBkgdOut();
	}
}

// Platform1A	8.00us
void LoopInput()
{
	BDM08 bdm;
	bdm.SetBkgdIn();
	volatile int b;
	for (;;)
	{
		bdm.SetReset(0);
		bdm.SetReset(0);
		bdm.SetReset(0);
		bdm.SetReset(0);
		bdm.SetReset(1);
		bdm.SetReset(1);
		bdm.SetReset(1);
		bdm.SetReset(1);
		b = bdm.GetBkgd();
		b = bdm.GetBkgd();
		b = bdm.GetBkgd();
		b = bdm.GetBkgd();
		b = bdm.GetBkgd();
		b = bdm.GetBkgd();
		b = bdm.GetBkgd();
		b = bdm.GetBkgd();
		b = bdm.GetBkgd();
		b = bdm.GetBkgd();
	}
}

void BdmTest()
{
	CriticalSection cs;
	BDM08 bdm;
//#ifdef WDT_EN
	CyGOS::KickWatchDog();
//#endif
	bdm.SetReset(1);
	CPU::DelayMs(6);
	bdm.SetReset(0);
	CPU::DelayMs(6);
	bdm.SetReset(1);
	CPU::DelayMs(6);
	//CyGOS::KickWatchDog();
	bdm.Sync();
	if(BDMExitFlag == FALSE)
	{
		bdm.Sync();
		bdm.Sync();
		//CyGOS::KickWatchDog();
		CPU::DelayMs(6);
		uint8 command[4];
		uint8 status[2];
		command[0] = 0xE4;
		bdm.Transfer(command, 1, status, 1);
		CPU::DelayMs(10);
		bdm.Send(0x90);
		CPU::DelayMs(10);
		bdm.Transfer(command, 1, status, 1);
		CPU::DelayMs(10);
		//CyGOS::KickWatchDog();
		pprintf("Setting BDCBKPT to 1234\n");
		command[0] = 0xC2;
		command[1] = 0x12;
		command[2] = 0x34;
		bdm.Transfer(command, 3, 0, 0);
		CPU::DelayMs(10);
		//CyGOS::KickWatchDog();
		command[0] = 0xE2;
		bdm.Transfer(command, 1, status, 2);
		pprintf("BDCBKPT is now %02X%02X\n", status[0], status[1]);
		pprintf("Setting BDCBKPT to 2345\n");
		command[0] = 0xC2;
		command[1] = 0x23;
		command[2] = 0x45;
		bdm.Transfer(command, 3, 0, 0);
		CPU::DelayMs(10);
		//CyGOS::KickWatchDog();
		command[0] = 0xE2;
		bdm.Transfer(command, 1, status, 2);
		pprintf("BDCBKPT is now %02X%02X\n", status[0], status[1]);
		CPU::DelayMs(10);
		//CyGOS::KickWatchDog();
		pprintf("Setting BDCSCR to C8\n");
		command[0] = 0xC4;
		command[1] = 0xC8;
		bdm.Transfer(command, 2, 0, 0);
		CPU::DelayMs(10);
		//CyGOS::KickWatchDog();
		command[0] = 0xE4;
		bdm.Transfer(command, 1, status, 1);
		pprintf("BDCSCR is now %02X\n", status[0]);
		CPU::DelayMs(10);
		//CyGOS::KickWatchDog();
		pprintf("Setting BDCSCR to 81\n");
		command[0] = 0xC4;
		command[1] = 0x81;
		bdm.Transfer(command, 2, 0, 0);
		CPU::DelayMs(10);
		command[0] = 0xE4;
		bdm.Transfer(command, 1, status, 1);
		pprintf("BDCSCR is now %02X\n", status[0]);
		CyGOS::KickWatchDog();
	}
}

uint8 something[] =
{ 0x87, 0xC6, 0x18, 0x25, 0xA4, 0x40, 0x27, 0xF9, 0x86, 0x81, 0x00, 0x8B, 0x89,
		0xF6, 0x45, 0x00, 0xB7, 0x87, 0xEB, 0x01, 0xE7, 0x01, 0x4F, 0xF9, 0xF7,
		0x86, 0xAE, 0x08, 0x9E, 0xE8, 0x03, 0x48, 0x24, 0x02, 0xA8, 0x07, 0x5A,
		0x26, 0xF8, 0x9E, 0xE7, 0x03, 0x88, 0x8A, 0x65, 0xFF, 0xFF, 0xAF, 0x01,
		0x26, 0xD8, 0x86, 0x82, 0x9D, 0x9D, 0x00, 0xFF, 0x00, 0xCD, 0xAB, 0xA6,
		0xA7, 0x06, 0x9E, };

void BdmBackground(BDM08 & bdm)
{
	printf("BACKGROUND\n");
	bdm.Background();
	CPU::DelayMs(5);
	CyGOS::KickWatchDog();
}

void BdmTestBreakpoint(BDM08 & bdm, int pattern)
{
	int bkpt = bdm.ReadBkpt();
	switch (pattern)
	{
	case 0x0555:
		bdm.WriteBkpt(0x0555);
		bdm.ReadBkpt();
		bdm.WriteBkpt(0x0AAA);
		bdm.ReadBkpt();
		break;
	case 0x1234:
		bdm.WriteBkpt(pattern);
		bdm.ReadBkpt();
		break;
/*	case 0x0000:
		bdm.WriteBkpt(pattern);
		bdm.ReadBkpt();
		break;
*/	}
	bdm.WriteBkpt(bkpt);
}

uint8 erase[] =
{ 0xC6, 0x18, 0x25, 0xAA, 0x30, 0xC7, 0x18, 0x25, 0xB6, 0x66, 0xF7, 0xB6, 0x67,
		0xC7, 0x18, 0x26, 0xA6, 0x80, 0xC7, 0x18, 0x25, 0x81, 0x87, 0xC6, 0x18,
		0x25, 0xA4, 0x40, 0x27, 0xF9, 0x86, 0x81, 0x45, 0x08, 0x1F, 0x94, 0x87,
		0xC6, 0x18, 0x25, 0xAA, 0x30, 0xC7, 0x18, 0x25, 0x86, 0xC7, 0x18, 0x20,
		0x45, 0x00, 0x00, 0x82, 0x00, };

void Expect(int read, int expect)
{
	if (read != expect){
		//pprintf("Read %02X, expected %02X\n", read, expect);
	}
}

void BdmProgram(UINT8 *dataBuffer, int dataCount)
{
	CriticalSection cs;
	//pprintf("\x0C");
	BDM08 bdm;
	bdm.diagnostics = false;
//	bdm.Reset();
	bdm.Sync();
	if(BDMExitFlag == FALSE)
	{
		//Push to BDM.

		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.WriteSCR(0x81);
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.WriteSCR(0x89);
		bdm.Sync();
		bdm.ReadSCR();
		BdmBackground(bdm);
		bdm.ReadSCR();
		bdm.Sync();
		BdmTestBreakpoint(bdm, 0x1234);
		bdm.ReadSCR();
		CyGOS::KickWatchDog();

		//Reset the Board.
		bdm.WriteByte(0x1801,0,0x01);	//Force Reset to MCU
		bdm.Sync();
		BdmTestBreakpoint(bdm, 0x1234);
		bdm.ReadSCR();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
//Read the Registers
		int pc = bdm.ReadPC();
		int sp = bdm.ReadSP();
		uint8 TrimReg[2] = {0,0};
		bdm.ReadByte(0xFFFE, TrimReg, 2);
		bdm.WritePC(pc);
		bdm.WriteSP(sp);
		BdmTestBreakpoint(bdm, 0x0555);
		bdm.Sync();
		bdm.ReadSCR();
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		int ccr = bdm.ReadCCR();
		int a = bdm.ReadA();
		int hx = bdm.ReadHX();
		bdm.ReadByte(0x1821, 0, 1);
		bdm.Sync();
		bdm.ReadByte(0x1810, 0, 1);
		bdm.ReadByte(0x1811, 0, 1);
		bdm.ReadByte(0x1812, 0, 1);
		bdm.ReadByte(0x1813, 0, 1);
		bdm.ReadByte(0x1814, 0, 1);
		bdm.ReadByte(0x1815, 0, 1);
		bdm.ReadByte(0x1818, 0, 1);
		bdm.ReadByte(0x1819, 0, 1);
		bdm.ReadByte(0x181A, 0, 1);
		bdm.ReadByte(0x181C, 0, 1);
		bdm.ReadByte(0x181D, 0, 1);
		bdm.WritePC(pc);
		bdm.WriteSP(sp);
		bdm.WriteCCR(ccr);
		bdm.WriteA(a);
		bdm.WriteHX(hx);
		CyGOS::KickWatchDog();

//Prepare for program
		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.Sync();
		BdmTestBreakpoint(bdm, 0x1234);
		bdm.ReadSCR();
		bdm.WriteByte(0x1801,0,0x01);	//Force Reset to MCU
		bdm.Sync();
		BdmTestBreakpoint(bdm, 0x1234);
		bdm.ReadSCR();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		CyGOS::KickWatchDog();
		bdm.ReadByte(0xFFFE, TrimReg, 2);
		bdm.WritePC(pc);
		bdm.WriteSP(sp);
		BdmTestBreakpoint(bdm, 0x0555);
		bdm.Sync();
		bdm.ReadSCR();
		BdmTestBreakpoint(bdm, 0x0555);
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		ccr = bdm.ReadCCR();
		a = bdm.ReadA();
		hx = bdm.ReadHX();
		bdm.ReadByte(0x1821, 0, 1);
		bdm.Sync();
		bdm.ReadByte(0x1806, 0, 1);
		bdm.ReadByte(0x1807, 0, 1);
		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.Sync();
		BdmTestBreakpoint(bdm, 0x1234);
		bdm.ReadSCR();
		bdm.WriteByte(0x1801,0,0x01);	//Force Reset to MCU
		bdm.Sync();
		BdmTestBreakpoint(bdm, 0x1234);
		bdm.ReadSCR();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		ccr = bdm.ReadCCR();
		a = bdm.ReadA();
		hx = bdm.ReadHX();
		bdm.WritePC(pc);
		bdm.WriteSP(sp);
		bdm.WriteCCR(ccr);
		bdm.WriteA(a);
		bdm.WriteHX(hx);
		CyGOS::KickWatchDog();
		bdm.ReadPC();
		bdm.ReadSP();
		bdm.ReadByte(0xFFFE, TrimReg, 2);
		bdm.WritePC(pc);
		bdm.WriteSP(sp);
		BdmTestBreakpoint(bdm, 0x0555);
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		BdmTestBreakpoint(bdm, 0x0555);
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		BdmTestBreakpoint(bdm, 0x0555);
		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		BdmTestBreakpoint(bdm, 0x1234);
		bdm.WriteByte(0x1801,0,0x01);	//Force Reset to MCU
		bdm.Sync();
		BdmTestBreakpoint(bdm, 0x1234);
		bdm.ReadSCR();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		ccr = bdm.ReadCCR();
		a = bdm.ReadA();
		hx = bdm.ReadHX();
		bdm.WritePC(pc);
		bdm.WriteSP(sp);
		bdm.WriteCCR(ccr);
		bdm.WriteA(a);
		bdm.WriteHX(hx);
		BdmTestBreakpoint(bdm, 0x0555);
		bdm.ReadPC();
		bdm.ReadSP();
		bdm.ReadByte(0xFFFE, TrimReg, 2);
		bdm.WritePC(pc);
		bdm.WriteSP(sp);
		BdmTestBreakpoint(bdm, 0x0555);
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		BdmTestBreakpoint(bdm, 0x0555);
		CyGOS::KickWatchDog();
		//Write Appropriate registers in the BDM Mode
//		bdm.WriteByte(0x1802, 0x00); //Disable WatchDog
//		bdm.WriteByte(0x1824, 0xFF);
//		bdm.WriteByte(0x1861, 0xC0);
//		bdm.Sync();
//		bdm.ReadByte(0x1863, 0, 6);

/*		bdm.WriteByte(0x1802, 0x00); //Disable WatchDog
		bdm.Sync();
		bdm.ReadByte(0x1810, 0, 6);
		bdm.ReadByte(0x1818, 0, 3);
		bdm.ReadByte(0x181C, 0, 2);
		bdm.ReadNext(0x0080, 0, 0x40);
		bdm.ReadNext(0x00C0, 0, 0x40);
		bdm.ReadNext(0x0100, 0, 0x40);
		bdm.ReadByte(0x0000, 0, 0x30);
		bdm.ReadNext(0x8A00, 0, 0x40);
		bdm.ReadNext(0x8A40, 0, 0x40);
		bdm.ReadByte(0x0000, 0, 0x80);
		bdm.ReadNext(0x0140, 0, 0x40);
		bdm.ReadByte(0x1800, 0, 0x16);
		bdm.ReadSCR();
		bdm.WriteNext(0x180F, 0xC2);
		bdm.ReadByte(0x180F, 0, 1);
		bdm.Sync();
		bdm.Sync();

		 bdm.WritePC(pc);
		 bdm.WriteSP(sp);
		 bdm.WriteCCR(ccr);
		 bdm.WriteA(a);
		 bdm.WriteHX(hx);
		 bdm.ReadByte(0x1806, 0, 2);
		 bdm.Sync();
		 bdm.Sync();
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.Sync();
		 BdmTestBreakpoint(bdm, 0x1234);
		 bdm.ReadSCR();
		 bdm.Reset10usBkgd();
		 bdm.Sync();
		 BdmTestBreakpoint(bdm, 0x1234);
		 bdm.ReadSCR();
		 bdm.Sync();
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 pc = bdm.ReadPC();
		 sp = bdm.ReadSP();
		 ccr = bdm.ReadCCR();
		 a = bdm.ReadA();
		 hx = bdm.ReadHX();
		 bdm.WritePC(pc);
		 bdm.WriteSP(sp);
		 bdm.WriteCCR(ccr);
		 bdm.WriteA(a);
		 bdm.WriteHX(hx);
		 pc = bdm.ReadPC();
		 sp = bdm.ReadSP();
		 int b1 = bdm.ReadByte(0xFFFE);
		 int b2 = bdm.ReadByte(0xFFFF);
		 bdm.WritePC((b1 << 8) | b2);
		 bdm.WriteSP(sp);
		 BdmTestBreakpoint(bdm, 0x0555);
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 BdmTestBreakpoint(bdm, 0x0555);
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 BdmTestBreakpoint(bdm, 0x0555);
		 bdm.Sync();
		 bdm.Sync();
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.Sync();
		 BdmTestBreakpoint(bdm, 0x1234);
		 bdm.ReadSCR();
		 bdm.WriteByte(0x1801, 0x01);
		 bdm.Reset10usBkgd();
		 bdm.Sync();
		 BdmTestBreakpoint(bdm, 0x1234);
		 bdm.ReadSCR();
		 bdm.Sync();
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 pc = bdm.ReadPC();
		 sp = bdm.ReadSP();
		 ccr = bdm.ReadCCR();
		 a = bdm.ReadA();
		 hx = bdm.ReadHX();
		 bdm.WritePC(pc);
		 bdm.WriteSP(sp);
		 bdm.WriteCCR(ccr);
		 bdm.WriteA(a);
		 bdm.WriteHX(hx);
		 pc = bdm.ReadPC();
		 sp = bdm.ReadSP();
		 b1 = bdm.ReadByte(0xFFFE);
		 b2 = bdm.ReadByte(0xFFFF);
		 bdm.WritePC((b1 << 8) | b2);
		 bdm.WriteSP(sp);
		 BdmTestBreakpoint(bdm, 0x0555);
		 */

		bdm.WriteByte(0x1802, 0x02);
		bdm.WriteByte(0x1824, 0xFF);
		bdm.WriteByte(0x1861, 0xC0);
		bdm.Sync();
		bdm.ReadByte(0x1863);
		bdm.WriteByte(0x1863, 0x10);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x80);
		bdm.Sync();
		bdm.ReadByte(0x1863);
		bdm.WriteByte(0x1863, 0x30);
		bdm.Sync();
		bdm.ReadByte(0x1863);
		bdm.WriteByte(0x1863, 0x10);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x87);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x97);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA3);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA5);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x80);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x81);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x82);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x83);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x84);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x85);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x86);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x87);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x88);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x89);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8A);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8B);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8C);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8D);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8E);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x90);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x91);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x92);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x93);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x94);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x95);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x96);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x97);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x98);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x99);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9A);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9B);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9C);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9D);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9E);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA0);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA1);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA2);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA3);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA4);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA5);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA6);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA7);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA8);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA9);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xAA);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xAB);
		bdm.Sync();
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA3);
		bdm.Sync();
		bdm.ReadByte(0x1863);
		bdm.WriteByte(0x1863, 0x11);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x80);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x81);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x82);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x83);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x84);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x85);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x86);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x87);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x88);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x89);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8A);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8B);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8C);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8D);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8E);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x90);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x91);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x92);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x93);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x94);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x95);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x96);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x97);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x98);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x99);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9A);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9B);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9C);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9D);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9E);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA0);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA1);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA2);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA3);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA4);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA5);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA6);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA7);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA8);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA9);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xAA);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xAB);
		bdm.Sync();
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA2);
		bdm.Sync();
		bdm.WriteByte(0x1861, 0x00);
		bdm.Sync();
		bdm.WriteByte(0x1863, 0x40);
		bdm.Sync();
		bdm.ReadByte(0x1821);
		bdm.Sync();
		bdm.WritePC(pc);
		bdm.WriteSP(sp);
		bdm.WriteCCR(ccr);
		bdm.WriteA(a);
		bdm.WriteHX(hx);
		bdm.ReadSCR();
		CyGOS::KickWatchDog();
		//Load Data to RAM
		pprintf("PrePare Erase\n");
		static uint8 codeA[] =
		{\
				0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x93, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x80, 0x00,\
				0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x60, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x93, 0x00, 0x00, 0x04, 0x93, 0x00, 0x00, 0x00, 0x00,\
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x93, 0x00, 0x00, 0x00, 0x00,\
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x93, 0x00, 0x00, 0x50, 0x93,\
		};
		bdm.WriteNext(0x006A, codeA, sizeof(codeA));
		bdm.ReadNext(0x7A, 0, 0x0F); // READ the RAM
		bdm.ReadNext(0x8A, 0, 0x0F); // READ the RAM
		bdm.ReadNext(0x9A, 0, 0x0F); // READ the RAM
		bdm.ReadNext(0xAA, 0, 0x0F); // READ the RAM
		bdm.ReadByte(0x006A, 0, 0x50); // READ the RAM
		CyGOS::KickWatchDog();

		pprintf("Erasing?\n");
		static uint8 codeB[] = \
		{\
				0xC6, 0x18, 0x25, 0xA4, 0x30, 0x27, 0x0A, 0xA6, 0x30, 0xC7, 0x18, 0x25, 0xA6, 0x30, 0xC7, 0x18,\
				0x25, 0xB6, 0x66, 0xF7, 0xB6, 0x67, 0xC7, 0x18, 0x26, 0xA6, 0x80, 0xC7, 0x18, 0x25, 0x81, 0x87,\
				0xC6, 0x18, 0x25, 0xA4, 0x40, 0x27, 0xF9, 0x86, 0x81, 0x45, 0x08, 0x1F, 0x94, 0x87, 0xC6, 0x18,\
				0x25, 0xAA, 0x30, 0xC7, 0x18, 0x25, 0x86, 0xC7, 0x18, 0x20, 0x45, 0x00, 0x00, 0x82 \
		};
		bdm.WriteNext(0x006A, codeB, sizeof(codeB));

		bdm.WritePC(0x0093);
		bdm.WriteSP(0x00FF);
		bdm.WriteCCR(0x68);
		bdm.WriteA(0x04);
		bdm.WriteHX(0x0004);
		CyGOS::KickWatchDog();
		bdm.Go();
		CyGOS::KickWatchDog();
		pprintf("E1");
		BDMExitFlag = TRUE;
		for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
		{
			CPU::DelayMs(10);
			int scr = bdm.ReadSCR();
			if (scr == 0xC8) {
				BDMExitFlag = FALSE;
				break;
			}
		}
		CyGOS::KickWatchDog();
		pprintf("E2");
		CPU::DelayMs(100);
		CyGOS::KickWatchDog();
		pprintf("E3");
		Expect(bdm.ReadSCR(), 0xC8);
		Expect(bdm.ReadPC(), 0x00A8);
		Expect(bdm.ReadSP(), 0x081E);
		Expect(bdm.ReadCCR(), 0x6A);
		Expect(bdm.ReadA(), 0x04);
		Expect(bdm.ReadHX(), 0x0000);
		Expect(bdm.ReadByte(0x1820), 0x84);
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		BdmTestBreakpoint(bdm, 0x0555);
		Expect(bdm.ReadByte(0x1821), 0xC2);
		bdm.ReadSCR();

//			bdm.diagnostics = true;
//			bdm.VerifyHXBlank(0x8000, 0xFF);
//			bdm.VerifyHXBlank(0xfff0, 0x0f);
//			bdm.diagnostics = false;
		pprintf("Erased?\n");

//Stage 2 Data on RAM
			static uint8 code2[] =
			{ 0x45, 0x80, 0x00, 0xA6, 0xFF, 0xB7, 0x66, 0xA6, 0x41, 0xB7, 0x67, 0xBD,
					0x6A, 0x45, 0x80, 0x00, 0xA6, 0xFF, 0xB7, 0x66, 0xA6, 0x05, 0xB7,
					0x67, 0xBD, 0x6A, 0xBD, 0x89, 0xA6, 0x20, 0xB7, 0x67, 0xA6, 0xFE,
					0xB7, 0x66, 0x45, 0xFF, 0xBF, 0xBD, 0x6A, 0xBD, 0x89, 0xCC, 0x00,
					0xC1, 0xA6, 0xFF, 0x45, 0x80, 0x00, 0x71, 0x03, 0xCC, 0x00, 0xE0,
					0x65, 0xFF, 0xBF, 0x26, 0xF6, 0x45, 0xFF, 0xC0, 0x71, 0x03, 0xCC,
					0x00, 0xE0, 0x65, 0x00, 0x00, 0x26, 0xF6, 0x45, 0x00, 0x00, 0x35,
					0x64, 0x82 };
			bdm.WriteNext(0x0093, code2, sizeof(code2));
			bdm.WriteByte(0x0062, 0x80);
			bdm.WriteByte(0x0063, 0x00);
			bdm.WriteByte(0x0064, 0x80);
			bdm.WriteByte(0x0065, 0x00);
			bdm.WritePC(0x0093);
			bdm.WriteSP(0x081E);
			CyGOS::KickWatchDog();
			bdm.Go();
			CyGOS::KickWatchDog();
			BDMExitFlag = TRUE;
			for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
			{
				CPU::DelayMs(10);
				int scr = bdm.ReadSCR();
				if (scr == 0xC8) {
					BDMExitFlag = FALSE;
					break;
				}
			}
			pprintf("Did something else?\n");

			Expect(bdm.ReadPC(), 0x00E3);
			Expect(bdm.ReadSP(), 0x081E);
			Expect(bdm.ReadCCR(), 0x6A);
			Expect(bdm.ReadA(), 0xFF);
			Expect(bdm.ReadHX(), 0x0000);
			Expect(bdm.ReadByte(0x0064), 0x00);
			Expect(bdm.ReadByte(0x0065), 0x00);
			Expect(bdm.ReadByte(0x1821), 0xC2);
			bdm.Sync();
			bdm.ReadSCR();
			bdm.ReadSCR();
			BdmTestBreakpoint(bdm, 0x0555);
			bdm.ReadSCR();
			CyGOS::KickWatchDog();

//Prepare For Programming Now
			bdm.PrepareProgram();
			CyGOS::KickWatchDog();
/*
			uint8 codeC[] =
			{
					0x8B, 0x89, 0x9E, 0xFE, 0x05, 0xF6, 0xAF, 0x01, 0x9E, 0xFF, 0x05, 0x88, 0x8A, 0x81, 0xA7, 0xFC,\
					0xC6, 0x80, 0x85, 0x4C, 0x95, 0xE7, 0x01, 0xC6, 0x80, 0x84, 0x4C, 0xF7, 0x32, 0x80, 0x86, 0x20,\
					0x1F, 0x89, 0x8B, 0xF6, 0x87, 0xE6, 0x02, 0x4C, 0x9E, 0xE7, 0x06, 0xE6, 0x03, 0xEE, 0x01, 0x8A,\
					0x4C, 0x20, 0x03, 0x7F, 0xAF, 0x01, 0x4B, 0xFB, 0x9E, 0x6B, 0x05, 0xF7, 0x8A, 0x88, 0xAF, 0x04,\
			};

			bdm.WriteNext(0x0093, codeC, sizeof(codeC));
			bdm.WriteByte(0x0060, 0x00);
			bdm.WriteByte(0x0061, 0x93);
			bdm.WriteByte(0x0062, 0x80);
			bdm.WriteByte(0x0063, 0x00);
			bdm.WriteByte(0x0064, 0x00);
			bdm.WriteByte(0x0065, 0x40);
			bdm.WritePC(0x0493);
			bdm.WriteSP(0x081E);
			bdm.WriteCCR(0x6A);
			bdm.WriteA(0xFF);
			bdm.WriteHX(0x0000);
			bdm.Go();

			BDMExitFlag = TRUE;
			for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
			{
				CPU::DelayMs(10);
				int scr = bdm.ReadSCR();
				if (scr == 0xC8) {
					BDMExitFlag = FALSE;
					break;
				}
			}

			Expect(bdm.ReadPC(), 0x04FD);
			Expect(bdm.ReadSP(), 0x081E);
			Expect(bdm.ReadCCR(), 0x6A);
			Expect(bdm.ReadA(), 0x04);
			Expect(bdm.ReadHX(), 0x0000);
			Expect(bdm.ReadByte(0x1821), 0xC2);
			bdm.ReadSCR();
*/

			//ARUN's Changes

			int i;
			int DestAddr = 0x8000;
			static UINT8* LocAddress = dataBuffer,*ReadBuffer = dataBuffer;
			int j;
			UINT8 Check = 0xFF;
			for(i=0;(i<(dataCount/64)) && (BDMExitFlag == FALSE);i++)
			{
				CyGOS::KickWatchDog();

				Check = 0xFF;			//Don't Program the empty blocks
				for( j = 0; j<64; j++)	//0x40 bytes or 64bytes written at once
				{
					Check &= *(LocAddress++);
				}
				if(Check != 0xFF) {
					printf(".");
					bdm.ProgramFlash(DestAddr, dataBuffer, 64);
				}
				else printf("Block Empty\n");
				DestAddr += 64; //0x40
				dataBuffer +=64; //0x40
				CPU::DelayMs(10);
				CyGOS::KickWatchDog();
			}
			Check = 0xFF;
			DestAddr = 0x8000;
			LocAddress = dataBuffer = ReadBuffer;

			printf("Verify \n");
			for(i=0;(i<(dataCount/64)) && (BDMExitFlag == FALSE);i++)
			{
				CyGOS::KickWatchDog();


				Check = 0xFF;			//Don't Program the empty blocks
				for( j = 0; j<64; j++)	//0x40 bytes or 64bytes written at once
				{
					Check &= *(LocAddress++);
				}
				if(Check != 0xFF) {
				//Read And Verify
					printf("#");
					BDMExitFlag = FALSE;
					bdm.VerifyHX(DestAddr,dataBuffer,64);
					if(BDMExitFlag == TRUE) {
						pprintf("FAIL to CHECK");
						printf("\n Data to Write  = %02X Data Read %02X  Address %04X\n", WriteData, ReadData,Address);
//						BDMExitFlag = FALSE;
//						extern UINT8 tempbuff[];
//						extern UINT8 tempbuffData[];
//						for(int z =0 ; z < 64; z++){
//							printf("\n %02X %02X ", tempbuff[z], tempbuffData[z]);
//						}
//						return;
					}
				}
				else printf("Block Empty\n");
				
				DestAddr += 64; //0x40
				dataBuffer +=64; //0x40
				CPU::DelayMs(10);
				CyGOS::KickWatchDog();
			}
			if(BDMExitFlag == FALSE)
			{
				printf("L0");
			//Do something ------- Extra
				bdm.ReadSCR();
				pc = bdm.ReadPC();
				sp = bdm.ReadSP();
				ccr = bdm.ReadCCR();
				a = bdm.ReadA();
				hx = bdm.ReadHX();
				bdm.Sync();
				bdm.ReadSCR();
				bdm.ReadSCR();
				BdmTestBreakpoint(bdm, 0x0555);
				printf("LA");
				CyGOS::KickWatchDog();
				BDMExitFlag = TRUE;
				for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
				{
					CPU::DelayMs(10);
					int scr = bdm.ReadSCR();
					if (scr == 0xC8) {
						BDMExitFlag = FALSE;
						break;
					}
				}
				WriteSomeMoreData(bdm);
				CyGOS::KickWatchDog();
				printf("L1");
				BdmTestBreakpoint(bdm, 0x0555);
				Expect(bdm.ReadByte(0xFFAF), 0xFF);
				Expect(bdm.ReadByte(0x1821), 0xC2);
				bdm.ReadNext(0xFBFF,0,0x210);
				bdm.ReadNext(0xFE0F,0,0x1F0);
				bdm.ReadSCR();
				CyGOS::KickWatchDog();

				bdm.WriteByte(0x0092,0,0x9D); //
				bdm.WriteByte(0x0060, 0x00);
				bdm.WriteByte(0x0061, 0x93);
				bdm.WriteByte(0x0062, 0xFF);
				bdm.WriteByte(0x0063, 0xAF);
				bdm.WriteByte(0x0064, 0x00);
				bdm.WriteByte(0x0065, 0x01);
				bdm.WritePC(0x0493);
				bdm.WriteSP(0x081E);
				bdm.WriteCCR(0x6A);
				bdm.WriteA(0xD0);
				bdm.WriteHX(0x0000);
				CyGOS::KickWatchDog();
				bdm.Go();
				CyGOS::KickWatchDog();
				printf("L2");
				BDMExitFlag = TRUE;
				for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
				{
					CPU::DelayMs(10);
					int scr = bdm.ReadSCR();
					if (scr == 0xC8) {
						BDMExitFlag = FALSE;
						break;
					}
				}
				CyGOS::KickWatchDog();
				printf("L3");
				Expect(bdm.ReadPC(), 0x04FD);
				Expect(bdm.ReadSP(), 0x081E);
				Expect(bdm.ReadCCR(), 0x6A);
				Expect(bdm.ReadA(), 0x9D);
				Expect(bdm.ReadHX(), 0x0000);
				Expect(bdm.ReadByte(0xFFAF), 0x9D);
				Expect(bdm.ReadByte(0x1821), 0xC2);

				bdm.ReadSCR();

				bdm.WriteByte(0x0092,0,0x01); //
				bdm.WriteByte(0x0060, 0x00);
				bdm.WriteByte(0x0061, 0x93);
				bdm.WriteByte(0x0062, 0xFF);
				bdm.WriteByte(0x0063, 0xAE);
				bdm.WriteByte(0x0064, 0x00);
				bdm.WriteByte(0x0065, 0x01);
				bdm.WritePC(0x0493);
				bdm.WriteSP(0x081E);
				bdm.WriteCCR(0x6A);
				bdm.WriteA(0x9D);
				bdm.WriteHX(0x0000);
				CyGOS::KickWatchDog();
				bdm.Go();
				CyGOS::KickWatchDog();
				printf("L4");
				BDMExitFlag = TRUE;
				for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
				{
					CPU::DelayMs(10);
					int scr = bdm.ReadSCR();
					if (scr == 0xC8) {
						BDMExitFlag = FALSE;
						break;
					}
				}

				CyGOS::KickWatchDog();

				printf("L5");
			//Change Complete
				/*
				Expect(bdm.ReadPC(), 0x04FD);
				Expect(bdm.ReadSP(), 0x081E);
				Expect(bdm.ReadCCR(), 0x6A);
				Expect(bdm.ReadA(), 0x01);
				Expect(bdm.ReadHX(), 0x0000);
				Expect(bdm.ReadByte(0xFFAE), 0x01);
				bdm.Sync();
				bdm.ReadSCR();
				bdm.ReadSCR();
				BdmTestBreakpoint(bdm, 0x0555);
				bdm.Sync();
				bdm.Sync();
				bdm.Sync();
				bdm.ReadSCR();
				bdm.ReadSCR();
				bdm.ReadSCR();
				bdm.ReadSCR();
				bdm.Sync();
				BdmTestBreakpoint(bdm, 0x1234);
				bdm.ReadSCR();
				bdm.WriteByte(0x1801,0,0x01);	//Force Reset to MCU
				bdm.Sync();
				BdmTestBreakpoint(bdm, 0x1234);
				bdm.ReadSCR();
				bdm.Sync();
				bdm.Sync();
				bdm.ReadSCR();
				bdm.ReadSCR();
				bdm.ReadSCR();
				bdm.ReadSCR();
				pc = bdm.ReadPC();
				sp = bdm.ReadSP();
				ccr = bdm.ReadCCR();
				a = bdm.ReadA();
				hx = bdm.ReadHX();
				bdm.WritePC(pc);
				bdm.WriteSP(sp);
				bdm.WriteCCR(ccr);
				bdm.WriteA(a);
				bdm.WriteHX(hx);
				bdm.ReadPC();
				bdm.ReadSP();
				Expect(bdm.ReadByte(0xFFFE), 0x80);
				Expect(bdm.ReadByte(0xFFFE), 0x7B);
				bdm.WritePC(pc);
				bdm.WriteSP(sp);
				BdmTestBreakpoint(bdm, 0x0555);
				bdm.Sync();
				bdm.ReadSCR();
				bdm.ReadSCR();
				BdmTestBreakpoint(bdm, 0x0555);
				bdm.Sync();
				bdm.ReadSCR();
				bdm.ReadSCR();
				BdmTestBreakpoint(bdm, 0x0555);
				bdm.Sync();
				bdm.Sync();
				bdm.Sync();*/
				bdm.ReadSCR();
				bdm.ReadSCR();
				bdm.ReadSCR();
				bdm.ReadSCR();
				bdm.Sync();
				BdmTestBreakpoint(bdm, 0x1234);
				bdm.ReadSCR();
				bdm.WriteByte(0x1801,0,0x01);	//Force Reset to MCU
				bdm.Sync();
				BdmTestBreakpoint(bdm, 0x1234);
				bdm.ReadSCR();
				bdm.Sync();
				bdm.Sync();
				bdm.ReadSCR();
				bdm.ReadSCR();
				bdm.ReadSCR();
				bdm.ReadSCR();
				pc = bdm.ReadPC();
				sp = bdm.ReadSP();
				Expect(bdm.ReadByte(0xFFFE), 0x80);
				Expect(bdm.ReadByte(0xFFFE), 0x7B);
				bdm.WritePC(pc);
				bdm.WriteSP(sp);
				BdmTestBreakpoint(bdm, 0x0555);
				pc = bdm.ReadPC();
				sp = bdm.ReadSP();
				ccr = bdm.ReadCCR();
				a = bdm.ReadA();
				hx = bdm.ReadHX();
				Expect(bdm.ReadByte(0x1821), 0xC2);
	//			bdm.ReadNext(0xFFC0,0,0x40);
				bdm.WritePC(pc);
				bdm.WriteSP(sp);
				bdm.WriteCCR(ccr);
				bdm.WriteA(a);
				bdm.WriteHX(hx);
				bdm.ReadPC();
	//			bdm.WriteByte(0x180E,0,0xC0);
				bdm.WritePC(0x807B);
				printf("L6");
				bdm.WriteSP(0x00FF);
				bdm.WriteCCR(0x68);
				bdm.WriteA(0x00);
				bdm.WriteHX(0x0000);
				CyGOS::KickWatchDog();
				bdm.Go();
				CyGOS::KickWatchDog();
			}

			bdm.diagnostics = true;
		//	bdm.Sync();
		//	bdm.PrepareCalculateChecksum();
		//	// Start of a region, end of the region + 1
		//	bdm.CalculateChecksum(0x8000, 32*1024);
		//
		//	bdm.Sync();
		//	BdmTestBreakpoint(bdm, 0x0555);
		//	Expect(bdm.ReadByte(0xFFAF), 0xFF);
		//	Expect(bdm.ReadByte(0x1821), 0xC2);
		//	bdm.VerifyHXBlank(0xFC00, 0x210);
		//	bdm.VerifyHX(0xFE10, vectors, 0x1F0);	//Verify Vector table
		//	bdm.diagnostics = true;
		//	bdm.Sync();
		//	bdm.ReadSCR();
		//	bdm.ReadSCR();
		//	BdmTestBreakpoint(bdm, 0x0555);
		//	bdm.Sync();
		//	bdm.ReadSCR();
		//	BdmTestBreakpoint(bdm, 0x1234);
		//	bdm.WriteByte(0x1801, 0x01);
		//	bdm.Reset10usBkgd();
		//	bdm.Sync();
		//	bdm.ReadSCR();
		//	BdmTestBreakpoint(bdm, 0x1234);
		//	bdm.Go();
			pprintf("Done\n");
		//}
	}
}


void WriteSomeMoreData(BDM08 bdm)
{
	printf("z");
	BDMExitFlag = TRUE;
	for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
	{
		CyGOS::KickWatchDog();
		int scr = bdm.ReadSCR();
		if (scr == 0xC8) {
			BDMExitFlag = FALSE;
			break;
		}
		CPU::DelayMs(10);
	}
	CyGOS::KickWatchDog();
	bdm.WriteByte(0x0093, 0x87);
	printf("y");
	bdm.WriteByte(0x0094, 0x8B);
	bdm.WriteByte(0x0095, 0x89);
	bdm.WriteByte(0x0096, 0xF6);
	Expect(bdm.ReadByte(0x0093), 0x87);
	Expect(bdm.ReadByte(0x0094), 0x8B);
	Expect(bdm.ReadByte(0x0095), 0x89);
	Expect(bdm.ReadByte(0x0096), 0xF6);
	CyGOS::KickWatchDog();
printf("a");
	bdm.WriteByte(0x0097, 0x45);
	bdm.WriteByte(0x0098, 0x00);
	bdm.WriteByte(0x0099, 0xC0);
	bdm.WriteByte(0x009A, 0x87);
	Expect(bdm.ReadByte(0x0097), 0x45);
	Expect(bdm.ReadByte(0x0098), 0x00);
	Expect(bdm.ReadByte(0x0099), 0xC0);
	Expect(bdm.ReadByte(0x009A), 0x87);

	bdm.WriteByte(0x009B, 0xEB);
	bdm.WriteByte(0x009C, 0x01);
	bdm.WriteByte(0x009D, 0xE7);
	bdm.WriteByte(0x009E, 0x01);
	Expect(bdm.ReadByte(0x009B), 0xEB);
	Expect(bdm.ReadByte(0x009C), 0x01);
	Expect(bdm.ReadByte(0x009D), 0xE7);
	Expect(bdm.ReadByte(0x009E), 0x01);

	bdm.WriteByte(0x009F, 0x4F);
	bdm.WriteByte(0x00A0, 0xF9);
	bdm.WriteByte(0x00A1, 0xF7);
	bdm.WriteByte(0x00A2, 0x86);
	Expect(bdm.ReadByte(0x009F), 0x4F);
	Expect(bdm.ReadByte(0x00A0), 0xF9);
	Expect(bdm.ReadByte(0x00A1), 0xF7);
	Expect(bdm.ReadByte(0x00A2), 0x86);
	CyGOS::KickWatchDog();
	printf("b");

	bdm.WriteByte(0x00A3, 0xAE);
	bdm.WriteByte(0x00A4, 0x08);
	bdm.WriteByte(0x00A5, 0x9E);
	bdm.WriteByte(0x00A6, 0xE8);
	Expect(bdm.ReadByte(0x00A3), 0xAE);
	Expect(bdm.ReadByte(0x00A4), 0x08);
	Expect(bdm.ReadByte(0x00A5), 0x9E);
	Expect(bdm.ReadByte(0x00A6), 0xE8);

	bdm.WriteByte(0x00A7, 0x03);
	bdm.WriteByte(0x00A8, 0x48);
	bdm.WriteByte(0x00A9, 0x24);
	bdm.WriteByte(0x00AA, 0x02);
	Expect(bdm.ReadByte(0x00A7), 0x03);
	Expect(bdm.ReadByte(0x00A8), 0x48);
	Expect(bdm.ReadByte(0x00A9), 0x24);
	Expect(bdm.ReadByte(0x00AA), 0x02);

	bdm.WriteByte(0x00AB, 0xA8);
	bdm.WriteByte(0x00AC, 0x07);
	bdm.WriteByte(0x00AD, 0x5A);
	bdm.WriteByte(0x00AE, 0x26);
	Expect(bdm.ReadByte(0x00AB), 0xA8);
	Expect(bdm.ReadByte(0x00AC), 0x07);
	Expect(bdm.ReadByte(0x00AD), 0x5A);
	Expect(bdm.ReadByte(0x00AE), 0x26);
	printf("c");

	bdm.WriteByte(0x00AF, 0xF8);
	bdm.WriteByte(0x00B0, 0x9E);
	bdm.WriteByte(0x00B1, 0xE7);
	bdm.WriteByte(0x00B2, 0x03);
	Expect(bdm.ReadByte(0x00AF), 0xF8);
	Expect(bdm.ReadByte(0x00B0), 0x9E);
	Expect(bdm.ReadByte(0x00B1), 0xE7);
	Expect(bdm.ReadByte(0x00B2), 0x03);

	bdm.WriteByte(0x00B3, 0x88);
	bdm.WriteByte(0x00B4, 0x8A);
	bdm.WriteByte(0x00B5, 0x65);
	bdm.WriteByte(0x00B6, 0xFF);
	Expect(bdm.ReadByte(0x00B3), 0x88);
	Expect(bdm.ReadByte(0x00B4), 0x8A);
	Expect(bdm.ReadByte(0x00B5), 0x65);
	Expect(bdm.ReadByte(0x00B6), 0xFF);
	CyGOS::KickWatchDog();

	bdm.WriteByte(0x00B7, 0xFF);
	bdm.WriteByte(0x00B8, 0xAF);
	bdm.WriteByte(0x00B9, 0x01);
	bdm.WriteByte(0x00BA, 0x26);
	Expect(bdm.ReadByte(0x00B7), 0xFF);
	Expect(bdm.ReadByte(0x00B8), 0xAF);
	Expect(bdm.ReadByte(0x00B9), 0x01);
	Expect(bdm.ReadByte(0x00BA), 0x26);

	bdm.WriteByte(0x00BB, 0xD8);
	bdm.WriteByte(0x00BC, 0x86);
	bdm.WriteByte(0x00BD, 0x82);
	bdm.WriteByte(0x00BE, 0x9D);
	Expect(bdm.ReadByte(0x00BB), 0xD8);
	Expect(bdm.ReadByte(0x00BC), 0x86);
	Expect(bdm.ReadByte(0x00BD), 0x82);
	Expect(bdm.ReadByte(0x00BE), 0x9D);
	CyGOS::KickWatchDog();

	bdm.WriteByte(0x00BF, 0x9D);
	bdm.WriteByte(0x00C0, 0x00);
	bdm.WriteByte(0x00C1, 0x00);
	bdm.WriteByte(0x00C2, 0x00);
	Expect(bdm.ReadByte(0x00BF), 0x9D);
	Expect(bdm.ReadByte(0x00C0), 0x00);
	Expect(bdm.ReadByte(0x00C1), 0x00);
	Expect(bdm.ReadByte(0x00C2), 0x00);

	bdm.WriteByte(0x00B6, 0xB8);
	bdm.WriteByte(0x00B7, 0xB8);
	bdm.WriteByte(0x00C0, 0x00);
	bdm.WriteByte(0x00C1, 0x00);
	Expect(bdm.ReadByte(0x00B6), 0xB8);
	Expect(bdm.ReadByte(0x00B7), 0xB8);
	Expect(bdm.ReadByte(0x00C0), 0x00);
	Expect(bdm.ReadByte(0x00C1), 0x00);
	printf("d");
	CyGOS::KickWatchDog();

	bdm.WritePC(0x0093);
	bdm.WriteSP(0x081E);
	bdm.WriteCCR(0x6A);
	bdm.WriteA(0x00);
	bdm.WriteHX(0x8000);
	printf("e");
	CyGOS::KickWatchDog();
	bdm.Go();
	CyGOS::KickWatchDog();

	BDMExitFlag = TRUE;
	for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
	{
		CPU::DelayMs(10);
		int scr = bdm.ReadSCR();
		if (scr == 0xC8) {
			BDMExitFlag = FALSE;
			CyGOS::KickWatchDog();
			break;
		}
	}
	CyGOS::KickWatchDog();
	printf("f");

	Expect(bdm.ReadPC(), 0x00BE);
	Expect(bdm.ReadSP(), 0x081E);
	Expect(bdm.ReadCCR(), 0x6A);
	Expect(bdm.ReadA(), 0xB7);
	Expect(bdm.ReadHX(), 0xB8B9);
	Expect(bdm.ReadByte(0x00C0), 0x6E);
	Expect(bdm.ReadByte(0x00C1), 0x2F);
	bdm.WriteByte(0x00B6, 0xFF);
	bdm.WriteByte(0x00B7, 0xEF);
	bdm.WriteByte(0x00C0, 0x00);
	bdm.WriteByte(0x00C1, 0x00);
	bdm.WritePC(0x0093);
	bdm.WriteSP(0x081E);
	bdm.WriteCCR(0x6A);
	bdm.WriteA(0x00);
	bdm.WriteHX(0xFFEE);
	CyGOS::KickWatchDog();
	bdm.Go();
	CyGOS::KickWatchDog();
	printf("g");

	BDMExitFlag = TRUE;
	for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
	{
		CPU::DelayMs(10);
		int scr = bdm.ReadSCR();
		if (scr == 0xC8) {
			BDMExitFlag = FALSE;
			CyGOS::KickWatchDog();
			break;
		}
	}
	CyGOS::KickWatchDog();
	printf("h");

	Expect(bdm.ReadPC(), 0x00BE);
	Expect(bdm.ReadSP(), 0x081E);
	Expect(bdm.ReadCCR(), 0x6A);
	Expect(bdm.ReadA(), 0x72);
	Expect(bdm.ReadHX(), 0xFFF0);
	Expect(bdm.ReadByte(0x00C0), 0x01);
	Expect(bdm.ReadByte(0x00C1), 0x94);
	bdm.WriteByte(0x00B6, 0xFF);
	bdm.WriteByte(0x00B7, 0xF3);
	bdm.WriteByte(0x00C0, 0x00);
	bdm.WriteByte(0x00C1, 0x00);
	bdm.WritePC(0x0093);
	bdm.WriteSP(0x081E);
	bdm.WriteCCR(0x6A);
	bdm.WriteA(0x00);
	bdm.WriteHX(0xFFF2);
	CyGOS::KickWatchDog();
	bdm.Go();
	CyGOS::KickWatchDog();
	printf("i");

	BDMExitFlag = TRUE;
	for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
	{
		CPU::DelayMs(10);
		int scr = bdm.ReadSCR();
		if (scr == 0xC8) {
			BDMExitFlag = FALSE;
			CyGOS::KickWatchDog();
			break;
		}
	}
	CyGOS::KickWatchDog();
	printf("j");

	Expect(bdm.ReadPC(), 0x00BE);
	Expect(bdm.ReadSP(), 0x081E);
	Expect(bdm.ReadCCR(), 0x6A);
	Expect(bdm.ReadA(), 0xCB);
	Expect(bdm.ReadHX(), 0xFFF4);
	Expect(bdm.ReadByte(0x00C0), 0x01);
	Expect(bdm.ReadByte(0x00C1), 0x62);
	bdm.WriteByte(0x00B6, 0xFF);
	bdm.WriteByte(0x00B7, 0xFF);
	bdm.WriteByte(0x00C0, 0x00);
	bdm.WriteByte(0x00C1, 0x00);
	bdm.WritePC(0x0093);
	bdm.WriteSP(0x081E);
	bdm.WriteCCR(0x6A);
	bdm.WriteA(0x00);
	bdm.WriteHX(0xFFFE);
	CyGOS::KickWatchDog();
	bdm.Go();
	CyGOS::KickWatchDog();
	printf("k");

	BDMExitFlag = TRUE;
	for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
	{
		CPU::DelayMs(10);
		int scr = bdm.ReadSCR();
		if (scr == 0xC8) {
			BDMExitFlag = FALSE;
			CyGOS::KickWatchDog();
			break;
		}
	}
	CyGOS::KickWatchDog();
	Expect(bdm.ReadPC(), 0x00BE);
	Expect(bdm.ReadSP(), 0x081E);
	Expect(bdm.ReadCCR(), 0x6A);
	Expect(bdm.ReadA(), 0xD0);
	Expect(bdm.ReadHX(), 0x0000);
	Expect(bdm.ReadByte(0x00C0), 0x00);
	Expect(bdm.ReadByte(0x00C1), 0xFB);
	bdm.Sync();
	bdm.ReadSCR();
	bdm.ReadSCR();
	CyGOS::KickWatchDog();
	printf("l");
}

#if 0
void BdmERASE(void)
{
	BDM08 bdm;
	bdm.diagnostics = true;

//	bdm.Reset();
	bdm.Sync();
	if(BDMExitFlag == FALSE)
	{
		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.WriteSCR(0x81);
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.WriteSCR(0x89);
		bdm.Sync();
		bdm.ReadSCR();
		BdmBackground(bdm);
		bdm.ReadSCR();
		bdm.Sync();
		BdmTestBreakpoint(bdm, 0x0555);
		bdm.ReadSCR();
		bdm.WriteByte(0x1801,0,0x01);	//Force Reset to MCU
		bdm.Sync();
		bdm.Sync();
		BdmTestBreakpoint(bdm, 0x1234);
		bdm.ReadSCR();
		//bdm.ReadByte(0x1801);
		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		while(bdm.ReadSCR() != 0xC8)
			printf(".");

		//Force to BDM Mode
/*		bdm.WriteSCR(0x81);
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.WriteSCR(0x89);
		bdm.Sync();
		bdm.ReadSCR();
		BdmBackground(bdm);
		bdm.ReadSCR();
		bdm.Sync();
		bdm.ReadSCR();
		while(bdm.ReadSCR() != 0xC8)
			printf(".");
		bdm.Sync();
*/
//In BDM Mode
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		BdmTestBreakpoint(bdm, 0x0555);
		//CPU::DelayUs(1);
		int pc = bdm.ReadPC();
		int sp = bdm.ReadSP();
		uint8 TrimReg[2] = {0,0};
		bdm.ReadByte(0xFFFE, TrimReg, 2);
		//pc = 0x807B;
		//sp = 0xFF;
		bdm.WritePC(pc);
		bdm.WriteSP(sp);
		BdmTestBreakpoint(bdm, 0x0555);
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		int ccr = bdm.ReadCCR();
		int a = bdm.ReadA();
		int hx = bdm.ReadHX();
		bdm.Sync();
		bdm.ReadSCR();
		BdmTestBreakpoint(bdm, 0x0555);
		//BdmTestBreakpoint(bdm, 0x0000);
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		ccr = bdm.ReadCCR();
		a = bdm.ReadA();
		hx = bdm.ReadHX();
		bdm.ReadByte(0x1821);
		bdm.Sync();
		bdm.ReadSCR();
		hx = bdm.ReadHX();
		//bdm.WriteHX(0x007f);

		static uint8 Do1[] =
		// 1	 2		3	4		5	6		7	8		9	A		B	C		D	E		F
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x2C, 0x00, 0xE7, 0xD4, 0x01, 0x00, 0x9B, 0xC6, 0x18, 0x02,
		  0xA4, 0x3F, 0xC7, 0x18, 0x02, 0x45, 0x00, 0xFC, 0x94, 0x0E, 0x8A, 0x3C, 0x55, 0x85, 0x96, 0X00,
		  0xBD, 0x55, 0x85, 0x89, 0x8B, 0x55, 0x88, 0x9F, 0x8B, 0x95, 0xEB, 0x02, 0xE7, 0x02, 0x86, 0xE9,
		  0x01, 0x87, 0xEE, 0x02, 0x8A, 0xAF, 0xFF, 0x96, 0x00, 0xC4, 0xA7, 0x02, 0x45, 0x00, 0x85, 0xF6,
		  0x4C, 0x26, 0x12, 0x65, 0xFF, 0xFF, 0x27, 0x09, 0xAF, 0x01, 0x65, 0xFF, 0xBF, 0x27, 0xF9, 0x20,
		  0xEE, 0xA6, 0x02, 0xB7, 0x8B, 0x82, 0x20, 0xFD, 0x45, 0x00, 0x00, 0xAF, 0x01, 0x35, 0x82, 0x65,
		  0x00, 0x00, 0x26, 0xF7, 0x55, 0x80, 0xAF, 0x01, 0x35, 0x80, 0x20, 0xEC, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
		uint8 Com0[1] = {0x80};
		bdm.ReadSCR();
		bdm.WriteNext(0x0080, Do1, sizeof(Do1));
		//bdm.WriteHX(0x0000);
		bdm.ReadSCR();
		//bdm.ReadHX();
		bdm.WriteNext(0x8A,Com0,sizeof(Com0));
		//bdm.WriteHX(0x0000);
		bdm.Sync();
		bdm.WritePC(0x008C);
		bdm.WriteSP(0x00FF);
		bdm.WriteCCR(0x68);
		bdm.WriteA(0x00);
		bdm.WriteHX(0x0000);
		bdm.Go();
		CPU::DelayMs(100);
/*		while(bdm.ReadSCR() != 0xC8)
			printf(".");
*/
//1st Step Done
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.Background();
		bdm.ReadSCR();
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		ccr = bdm.ReadCCR();
		a = bdm.ReadA();
		hx = bdm.ReadHX();
		bdm.ReadByte(0x1821, 0, 1);
		//hx = bdm.ReadHX();
		bdm.ReadNext(0x0080,0, 0x40);		//read 64bytes of data from 7F
		//bdm.WriteHX(0x30FD);
		bdm.ReadSCR();
		//bdm.ReadHX();
		bdm.WriteNext(0x0080,Do1,sizeof(Do1));
		//bdm.WriteHX(hx);
		bdm.ReadSCR();
		//bdm.ReadHX();
		uint8 Com1[] = {0x14,0x43};
		bdm.WriteNext(0x0080, Com1, sizeof(Com1));
		bdm.ReadSCR();
		//bdm.ReadHX();
		uint8 Com2[] = {0x00,0x00,0x80,0x00,0x00,0x40,0x00};
		bdm.WriteNext(0x0084, &Com2[1], 6);
		bdm.WritePC(0x008C);
		bdm.WriteSP(0x00FB);
		bdm.WriteCCR(0x68);
		bdm.WriteA(0x02);
		bdm.WriteHX(hx);
		bdm.Go();
		CPU::DelayMs(100);
		while(bdm.ReadSCR() != 0xC8)
			printf(".");

		//Step2
		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.Sync();
		bdm.ReadSCR();
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		ccr = bdm.ReadCCR();
		a = bdm.ReadA();
		hx = bdm.ReadHX();
		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadByte(0x1821, 0, 1);
		//hx = bdm.ReadHX();
		bdm.ReadNext(0x0080,0,0x40);
		bdm.ReadSCR();
		uint8 Do2[] = {\
		0x00,0x10,0x20,0x00,0x26,0x30,0x01,0x00,0x00,0x00,0x9B,0xC6,0x18,0x02,0xA4,0x3F,\
		0xC7,0x18,0x02,0x45,0x01,0x6A,0x94,0xB6,0x86,0xA4,0x01,0x26,0x05,0x4A,0xB7,0x87,0x20,\
		0x2C,0xAD,0x2D,0x55,0x81,0x96,0x01,0x26,0x55,0x81,0x89,0x8B,0x55,0x84,0x9F,0x8B,\
		0x95,0xEB,0x02,0xE7,0x02,0x86,0xE9,0x01,0x87,0xEE,0x02,0x8A,0xAF,0xFF,0x96,0x01,\
		0x57,0xA7,0x02,0xAD,0x53,0xAD,0x2E,0x20,0x02,0x3C,0x87,0x3C,0x87,0x82,0x20,0xFD,0xC6,0x18,\
		0x20,0xA4,0x80,0x27,0x01,0x81,0xA6,0x30,0xC7,0x18,0x25,0x55,0x88,0xB6,0x89,0xAE,\
		0xC8,0x52,0x26,0x02,0x20,0x09,0xA1,0x3F,0x23,0x05,0x44,0x44,0x44,0xAA,0x40,0xC7,\
		0x18,0x20,0x81,0xB6,0x86,0xA4,0x20,0x27,0x01,0x81,0xC6,0xFF,0xBF,0xA4,0x03,0xA1,\
		0x02,0x27,0x12,0xA6,0x20,0xC7,0x01,0x29,0x45,0xFF,0xBF,0x96,0x01,0x26,0xA6,0xFF,\
		0xC7,0x01,0x57,0xAD,0x01,0x81,0xA6,0xFF,0xC7,0x18,0x24,0xA6,0x30,0xC7,0x18,0x25,\
		0xA6,0xFE,0xC7,0x12,0x34,0xA6,0x40,0xC7,0x18,0x26,0xA6,0x80,0xC7,0x18,0x25,0xC6,\
		0x18,0x25,0xA4,0x30,0x27,0x02,0x20,0x8F,0xC6,0x18,0x25,0xA4,0x40,0x27,0xF9,0xC6,\
		0x18,0x25,0xA4,0x80,0x27,0xF2,0xC6,0x01,0x26,0xAB,0x02,0x25,0x0D,0xC7,0x01,0x26,\
		0x32,0x01,0x26,0x65,0x12,0x34,0x22,0x02,0x20,0xBC,0x81,0x00,0x00,0x00,0x00,0x00,\
		0x00,0x00,0x00,0x00,0x00,0x00,0x00};

		bdm.WriteNext(0x0080, Do2, sizeof(Do2));
		bdm.ReadSCR();
		uint8 Com3[] = {0x00,0x80,0x00,0x00,0x40,0x00,0x01,0x00,0x14,0x43};
		bdm.WriteNext(0x0080,Com3,sizeof(Com3));
		bdm.WritePC(0x008A);
		bdm.WriteSP(0x00FB);
		bdm.WriteCCR(0x6C);
		bdm.WriteA(0x8C);
		bdm.WriteHX(0x8000);
		bdm.Go();
		//CPU::DelayMs(100);
		//3Step
		while(bdm.ReadSCR() != 0xC8)
			printf(".");

		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.Sync();
		bdm.ReadSCR();
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		ccr = bdm.ReadCCR();
		a = bdm.ReadA();
		hx = bdm.ReadHX();
		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadByte(0x1821, 0, 1);
		hx = bdm.ReadHX();
		bdm.ReadNext(0x0080,0,0x40);
		bdm.ReadSCR();
		uint8 Do3[] = {\
		0x00,0x00,0x00,0x00,0x00,0x18,0x2C,0x00,0xE7,0xD4,0x01,0x00,0x9B,0xC6,0x18,0x02,\
		0xA4,0x3F,0xC7,0x18,0x02,0x45,0x00,0xFC,0x94,0x0E,0x8A,0x3C,0x55,0x85,0x96,0x00,\
		0xBD,0x55,0x85,0x89,0x8B,0x55,0x88,0x9F,0x8B,0x95,0xEB,0x02,0xE7,0x02,0x86,0xE9,\
		0x01,0x87,0xEE,0x02,0x8A,0xAF,0xFF,0x96,0x00,0xC4,0xA7,0x02,0x45,0x00,0x85,0xF6,\
		0x4C,0x26,0x12,0x65,0xFF,0xFF,0x27,0x09,0xAF,0x01,0x65,0xFF,0xBF,0x27,0xF9,0x20,\
		0xEE,0xA6,0x02,0xB7,0x8B,0x82,0x20,0xFD,0x45,0x00,0x00,0xAF,0x01,0x35,0x82,0x65,\
		0x00,0x00,0x26,0xF7,0x55,0x80,0xAF,0x01,0x35,0x80,0x20,0xEC,0x00,0x00,0x00,0x00,\
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

		bdm.WriteNext(0x0080,Do3,sizeof(Do3));
		bdm.ReadSCR();
		//hx = bdm.ReadHX();
		bdm.WriteNext(0x0080,&Com3[8],2);
		bdm.ReadSCR();
		hx = bdm.ReadHX();

		bdm.WriteNext(0x0084,Com3,6);
		bdm.WritePC(0x008C);
		bdm.WriteSP(0x0169);
		bdm.WriteCCR(0x78);
		bdm.WriteA(0x02);
		bdm.WriteHX(0xC000);
		bdm.Go();
		CPU::DelayMs(100);
		while(bdm.ReadSCR() != 0xC8)
			printf(".");

		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.Sync();
		bdm.ReadSCR();
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		ccr = bdm.ReadCCR();
		a = bdm.ReadA();
		hx = bdm.ReadHX();
		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadByte(0x1821, 0, 1);
		hx = bdm.ReadHX();
		bdm.ReadNext(0x0080,0,0x40);
		bdm.ReadSCR();
		hx = bdm.ReadHX();
//		bdm.ReadNext(0x007F,0,0x40);
/*
		uint8 Do4[] = {\
		0x00,0x00,0x00,0x00,0x00,0x18,0x2C,0x00,0xE7,0xD4,0x01,0x00,0x9B,0xC6,0x18,0x02,\
		0xA4,0x3F,0xC7,0x18,0x02,0x45,0x00,0xFC,0x94,0x0E,0x8A,0x3C,0x55,0x85,0x96,0x00,\
		0xBD,0x55,0x85,0x89,0x8B,0x55,0x88,0x9F,0x8B,0x95,0xEB,0x02,0xE7,0x02,0x86,0xE9,\
		};
*/
		bdm.WriteNext(0x0080,Do3,sizeof(Do3));
		bdm.ReadSCR();
		hx = bdm.ReadHX();
		bdm.WriteNext(0x0080,&Com3[8],2);
		bdm.ReadSCR();
		hx = bdm.ReadHX();

		bdm.WriteNext(0x0084,Com3,6);
		bdm.WritePC(0x008C);
		bdm.WriteSP(0x0FB);
		bdm.WriteCCR(0x68);
		bdm.WriteA(0x02);
		bdm.WriteHX(0xBFFF);
		bdm.Go();
		CPU::DelayMs(100);
		while(bdm.ReadSCR() != 0xC8)
			printf(".");

		bdm.Sync();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.Sync();
		bdm.ReadSCR();
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		ccr = bdm.ReadCCR();
		a = bdm.ReadA();
		hx = bdm.ReadHX();
		bdm.Sync();
		bdm.Sync();
		bdm.Sync();
		bdm.ReadByte(0x1821, 0, 1);

		bdm.Sync();
		bdm.ReadSCR();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.Sync();
		bdm.ReadSCR();
		bdm.Reset10usBkgd();

	}
}



/*
		bdm.WriteSCR(0x81);
		bdm.ReadSCR();
		bdm.ReadSCR();
		bdm.WriteSCR(0x89);
		bdm.Sync();
		bdm.ReadSCR();
		BdmBackground(bdm);
		bdm.ReadSCR();
		bdm.Sync();
		bdm.ReadSCR();
		BdmTestBreakpoint(bdm, 0x0555);
		//bdm.diagnostics = true;//false;
		bdm.ReadByte(0x1821, 0, 1);
		bdm.Sync();
		bdm.ReadByte(0x1810, 0, 6);
		bdm.ReadByte(0x1818, 0, 3);
		bdm.ReadByte(0x181C, 0, 2);
		bdm.ReadNext(0x0080, 0, 0x40);
		bdm.ReadNext(0x00C0, 0, 0x40);
		bdm.ReadNext(0x0100, 0, 0x40);
		bdm.ReadByte(0x0000, 0, 0x30);
		bdm.ReadNext(0x8A00, 0, 0x40);
		bdm.ReadNext(0x8A40, 0, 0x40);
		bdm.ReadByte(0x0000, 0, 0x80);
		bdm.ReadNext(0x0140, 0, 0x40);
		bdm.ReadByte(0x1800, 0, 0x16);
		bdm.ReadSCR();
		bdm.WriteNext(0x180F, 0xC2);
		bdm.ReadByte(0x180F, 0, 1);
		bdm.Sync();
		bdm.Sync();
		/*
		 bdm.WritePC(pc);
		 bdm.WriteSP(sp);
		 bdm.WriteCCR(ccr);
		 bdm.WriteA(a);
		 bdm.WriteHX(hx);
		 bdm.ReadByte(0x1806, 0, 2);
		 bdm.Sync();
		 bdm.Sync();
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.Sync();
		 BdmTestBreakpoint(bdm, 0x1234);
		 bdm.ReadSCR();
		 bdm.Reset10usBkgd();
		 bdm.Sync();
		 BdmTestBreakpoint(bdm, 0x1234);
		 bdm.ReadSCR();
		 bdm.Sync();
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 pc = bdm.ReadPC();
		 sp = bdm.ReadSP();
		 ccr = bdm.ReadCCR();
		 a = bdm.ReadA();
		 hx = bdm.ReadHX();
		 bdm.WritePC(pc);
		 bdm.WriteSP(sp);
		 bdm.WriteCCR(ccr);
		 bdm.WriteA(a);
		 bdm.WriteHX(hx);
		 pc = bdm.ReadPC();
		 sp = bdm.ReadSP();
		 int b1 = bdm.ReadByte(0xFFFE);
		 int b2 = bdm.ReadByte(0xFFFF);
		 bdm.WritePC((b1 << 8) | b2);
		 bdm.WriteSP(sp);
		 BdmTestBreakpoint(bdm, 0x0555);
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 BdmTestBreakpoint(bdm, 0x0555);
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 BdmTestBreakpoint(bdm, 0x0555);
		 bdm.Sync();
		 bdm.Sync();
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.Sync();
		 BdmTestBreakpoint(bdm, 0x1234);
		 bdm.ReadSCR();
		 bdm.WriteByte(0x1801, 0x01);
		 bdm.Reset10usBkgd();
		 bdm.Sync();
		 BdmTestBreakpoint(bdm, 0x1234);
		 bdm.ReadSCR();
		 bdm.Sync();
		 bdm.Sync();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 bdm.ReadSCR();
		 pc = bdm.ReadPC();
		 sp = bdm.ReadSP();
		 ccr = bdm.ReadCCR();
		 a = bdm.ReadA();
		 hx = bdm.ReadHX();
		 bdm.WritePC(pc);
		 bdm.WriteSP(sp);
		 bdm.WriteCCR(ccr);
		 bdm.WriteA(a);
		 bdm.WriteHX(hx);
		 pc = bdm.ReadPC();
		 sp = bdm.ReadSP();
		 b1 = bdm.ReadByte(0xFFFE);
		 b2 = bdm.ReadByte(0xFFFF);
		 bdm.WritePC((b1 << 8) | b2);
		 bdm.WriteSP(sp);
		 BdmTestBreakpoint(bdm, 0x0555);
		 */
/*		bdm.Sync();
		bdm.ReadSCR();
		bdm.ReadSCR();
		BdmTestBreakpoint(bdm, 0x0555);
		bdm.WriteByte(0x1802, 0x03);
		bdm.WriteByte(0x1824, 0xFF);
		bdm.WriteByte(0x1861, 0xC0);
		bdm.Sync();
		bdm.ReadByte(0x1863);
		bdm.WriteByte(0x1863, 0x10);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x80);
		bdm.Sync();
		bdm.ReadByte(0x1863);
		bdm.WriteByte(0x1863, 0x30);
		bdm.Sync();
		bdm.ReadByte(0x1863);
		bdm.WriteByte(0x1863, 0x10);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x87);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x97);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA7);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xAB);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x80);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x81);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x82);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x83);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x84);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x85);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x86);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x87);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x88);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x89);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8A);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8B);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8C);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8D);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8E);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x90);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x91);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x92);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x93);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x94);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x95);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x96);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x97);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x98);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x99);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9A);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9B);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9C);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9D);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9E);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA0);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA1);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA2);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA3);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA4);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA5);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA6);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA7);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA8);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA9);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xAA);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xAB);
		bdm.Sync();
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA3);
		bdm.Sync();
		bdm.ReadByte(0x1863);
		bdm.WriteByte(0x1863, 0x11);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x80);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x81);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x82);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x83);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x84);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x85);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x86);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x87);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x88);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x89);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8A);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8B);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8C);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8D);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8E);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x8F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x90);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x91);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x92);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x93);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x94);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x95);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x96);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x97);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x98);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x99);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9A);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9B);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9C);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9D);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9E);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0x9F);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA0);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA1);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA2);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA3);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA4);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA5);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA6);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA7);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA8);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA9);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xAA);
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xAB);
		bdm.Sync();
		bdm.Sync();
		bdm.WriteByte(0x1862, 0xA2);
		bdm.Sync();
		bdm.WriteByte(0x1861, 0x00);
		bdm.Sync();
		bdm.WriteByte(0x1863, 0x40);
		bdm.Sync();
		bdm.ReadByte(0x1821);
		bdm.Sync();
		bdm.WritePC(pc);
		bdm.WriteSP(sp);
		bdm.WriteCCR(ccr);
		bdm.WriteA(a);
		bdm.WriteHX(hx);

		pprintf("Erasing?\n");
		static uint8 code1[] =
		{ 0xC6, 0x18, 0x25, 0xAA, 0x30, 0xC7, 0x18, 0x25, 0xB6, 0x66, 0xF7, 0xB6,
				0x67, 0xC7, 0x18, 0x26, 0xA6, 0x80, 0xC7, 0x18, 0x25, 0x81, 0x87,
				0xC6, 0x18, 0x25, 0xA4, 0x40, 0x27, 0xF9, 0x86, 0x81, 0x45, 0x08,
				0x1F, 0x94, 0x87, 0xC6, 0x18, 0x25, 0xAA, 0x30, 0xC7, 0x18, 0x25,
				0x86, 0xC7, 0x18, 0x20, 0x45, 0x00, 0x00 };
		bdm.WriteByte(0x006A, code1, sizeof(code1));
		int b1 = bdm.ReadByte(0x009E);
		int b2 = bdm.ReadByte(0x009F);
		bdm.WriteByte(0x009E, 0x82);
		bdm.WriteByte(0x009F, 0x00);
		bdm.ReadSCR();
		bdm.WritePC(0x008A);
		bdm.WriteSP(0x00FF);
		bdm.WriteCCR(0x68);
		bdm.WriteA(0x4B);
		bdm.WriteHX(0x0045);
		bdm.Go();
		CPU::DelayMs(100);
		bdm.Sync();
		if(BDMExitFlag == FALSE) {
			Expect(bdm.ReadSCR(), 0xC8);
			Expect(bdm.ReadPC(), 0x009F);
			Expect(bdm.ReadSP(), 0x081E);
			Expect(bdm.ReadCCR(), 0x6A);
			Expect(bdm.ReadA(), 0x4B);
			Expect(bdm.ReadHX(), 0x0000);
			Expect(bdm.ReadByte(0x1820), 0xCB);
			bdm.Sync();
			bdm.ReadSCR();
			bdm.ReadSCR();
			BdmTestBreakpoint(bdm, 0x0555);
			Expect(bdm.ReadByte(0x1821), 0xC2);

			bdm.diagnostics = true;
			bdm.VerifyHXBlank(0x8000, 0xFF);
			bdm.VerifyHXBlank(0xfff0, 0x0f);
			bdm.diagnostics = false;

			pprintf("Erased?\n");
			static uint8 code2[] =
			{ 0x45, 0x80, 0x00, 0xA6, 0xFF, 0xB7, 0x66, 0xA6, 0x41, 0xB7, 0x67, 0xBD,
					0x6A, 0x45, 0x80, 0x00, 0xA6, 0xFF, 0xB7, 0x66, 0xA6, 0x05, 0xB7,
					0x67, 0xBD, 0x6A, 0xBD, 0x80, 0xA6, 0x20, 0xB7, 0x67, 0xA6, 0xFE,
					0xB7, 0x66, 0x45, 0xFF, 0xBF, 0xBD, 0x6A, 0xBD, 0x80, 0xCC, 0x00,
					0xB8, 0xA6, 0xFF, 0x45, 0x80, 0x00, 0x71, 0x03, 0xCC, 0x00, 0xD7,
					0x65, 0xFF, 0xBF, 0x26, 0xF6, 0x45, 0xFF, 0xC0, 0x71, 0x03, 0xCC,
					0x00, 0xD7, 0x65, 0x00, 0x00, 0x26, 0xF6, 0x45, 0x00, 0x00, 0x35,
					0x64, 0x82 };
			bdm.WriteByte(0x008A, code2, sizeof(code2));
			bdm.WriteByte(0x0062, 0x80);
			bdm.WriteByte(0x0063, 0x00);
			bdm.WriteByte(0x0064, 0x80);
			bdm.WriteByte(0x0065, 0x00);
			bdm.WritePC(0x008A);
			bdm.WriteSP(0x081E);
			bdm.Go();
			BDMExitFlag = TRUE;
			for (LoopCount = 0;LoopCount < COUNT_MAX;LoopCount++)
			{
				CPU::DelayMs(10);
				int scr = bdm.ReadSCR();
				if (scr == 0xC8) {
					BDMExitFlag = FALSE;
					break;
				}
			}
			pprintf("Did something else?\n");
			Expect(bdm.ReadPC(), 0x00DA);
			Expect(bdm.ReadSP(), 0x081E);
			Expect(bdm.ReadCCR(), 0x6A);
			Expect(bdm.ReadA(), 0xFF);
			Expect(bdm.ReadHX(), 0x0000);
			Expect(bdm.ReadByte(0x0064), 0x00);
			Expect(bdm.ReadByte(0x0065), 0x00);
			Expect(bdm.ReadByte(0x1821), 0xC2);
			bdm.PrepareProgram();
			bdm.ReadSCR();
			BdmTestBreakpoint(bdm, 0x0555);
			bdm.ReadSCR();
			//ARUN's Changes

			int i;
			uint8 code4[1024] = {0};
			//char *SrcAddr = (char*)dataBuffer;
			int DestAddr = 0x8000;
			for(i=0;(i<(dataCount/1024)) && (BDMExitFlag == FALSE);i++)
			{
				printf(".");
				//pprintf("%d 1K Block written BufferAddress = %x PROGRAM MEMORY Address = %x\n",i,dataBuffer,DestAddr);
				//memcpy(code4,SrcAddr,1024); //copy 1K from data buffer to code4
				//SrcAddr +=1024;	//1K increment

				bdm.ProgramFlash(DestAddr, dataBuffer, 1024);//sizeof(code4));
				DestAddr += 1024; //1K increment
				dataBuffer +=1024;
			}



//			bdm.diagnostics = true;
//			SrcAddr = (char*)dataBuffer;
//			DestAddr = 0x8000;
//			bdm.VerifyHX(DestAddr, (uint8*)SrcAddr, 0x8000);	//Verify Vector table



		//Change Complete

			bdm.diagnostics = true;
		//	bdm.Sync();
		//	bdm.PrepareCalculateChecksum();
		//	// Start of a region, end of the region + 1
		//	bdm.CalculateChecksum(0x8000, 32*1024);
		//
		//	bdm.Sync();
		//	BdmTestBreakpoint(bdm, 0x0555);
		//	Expect(bdm.ReadByte(0xFFAF), 0xFF);
		//	Expect(bdm.ReadByte(0x1821), 0xC2);
		//	bdm.VerifyHXBlank(0xFC00, 0x210);
		//	bdm.VerifyHX(0xFE10, vectors, 0x1F0);	//Verify Vector table
		//	bdm.diagnostics = true;
		//	bdm.Sync();
		//	bdm.ReadSCR();
		//	bdm.ReadSCR();
		//	BdmTestBreakpoint(bdm, 0x0555);
		//	bdm.Sync();
		//	bdm.ReadSCR();
		//	BdmTestBreakpoint(bdm, 0x1234);
		//	bdm.WriteByte(0x1801, 0x01);
		//	bdm.Reset10usBkgd();
		//	bdm.Sync();
		//	bdm.ReadSCR();
		//	BdmTestBreakpoint(bdm, 0x1234);
		//	bdm.Go();
			pprintf("Done\n");
		}
	}
}
*/
#endif
