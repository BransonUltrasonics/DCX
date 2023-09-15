/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BDMTask.cpp_v   1.5   21 Sep 2015 06:45:30   RJamloki  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BDMTask.cpp_v  $
 * 
 *    Rev 1.5   21 Sep 2015 06:45:30   RJamloki
 * Added BDMProgramWithMassEraseAndBlankCheck() to disable the security and program the target processor.
 * 
 *    Rev 1.4   19 Mar 2015 06:34:08   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.3   28 Jan 2015 06:33:10   sesharma
 * Redefined printf and pprintf to Remove Compiler Warnings
 * 
 *    Rev 1.2   07 Nov 2014 15:25:24   ewomack
 * Enable printing
 * 
 *    Rev 1.1   17 Sep 2014 08:41:20   amaurya
 * Added Comments, code review changes fixes.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:20   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:56   amaurya
 * Initial revision.
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
#include "portable.h"
#include "BDMTask.h"

#define printf PrintF
//#define pprintf

extern UINT16 LoopCount;
extern UINT8 WriteData, ReadData;
extern SINT32 Address;
extern int DelayVal;

void BdmBackground(BDM08 & bdm)
/***************************************************************************************/
/*Description:
 This function is called from BdmProgram() where it enter active background mode.*/
/***************************************************************************************/
{
	printf("BACKGROUND\n");
	bdm.Background();
	CPU::DelayMs(5);
	CyGOS::KickWatchDog();
}

void BdmTestBreakpoint(BDM08 & bdm, SINT32 pattern)
/***************************************************************************************/
/*Description:
 This function write BDCBKPT breakpoint register and read depending on the pattern.*/
/***************************************************************************************/
{
	SINT32 bkpt = bdm.ReadBkpt();
	switch (pattern) {
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
	}
	bdm.WriteBkpt(bkpt);
}

UINT8 erase[] = { 0xC6, 0x18, 0x25, 0xAA, 0x30, 0xC7, 0x18, 0x25, 0xB6, 0x66,
      0xF7, 0xB6, 0x67, 0xC7, 0x18, 0x26, 0xA6, 0x80, 0xC7, 0x18, 0x25, 0x81,
      0x87, 0xC6, 0x18, 0x25, 0xA4, 0x40, 0x27, 0xF9, 0x86, 0x81, 0x45, 0x08,
      0x1F, 0x94, 0x87, 0xC6, 0x18, 0x25, 0xAA, 0x30, 0xC7, 0x18, 0x25, 0x86,
      0xC7, 0x18, 0x20, 0x45, 0x00, 0x00, 0x82, 0x00, };

void Expect(SINT32 read, SINT32 expect)
/***************************************************************************************/
/*Description:
 This function is for debugging while bdm program*/
/***************************************************************************************/
{
	if (read != expect) {
		//pprintf("Read %02X, expected %02X\n", read, expect);
	}
}

/*
 * void BDMProgramWithMassEraseAndBlankCheck(BDM08 & bdm, UINT8 * FileDataStart)
 *
 * Purpose:
 * 		To disable the security of target MC9S08LG32 processor using background debug command interface
 * 		and program the target.
 * 		Below steps are performed for the same.
 * 		1. Set the target to active background mode
 * 		2. Disable any block protections by writing to the FPROT register.
 * 		3. Execute Mass Erase FLASH, if necessary.
 * 		4. Execute Blank Check operation.
 * 		5. Program the target's flash
 * 		6. verify the programmed file.
 * 		7. Reset the LCD.
 *
 * 	Entry Condition:
 * 		LCD programming is initiated from web. Target's flash size is assumed to be 32K.
 * 		FileDataStart- Pointer to the file data buffer which needs to programmed on target's flash.
 * 	Exit Condition:
 * 		Target is unsecured and programmed.
 */
void BDMProgramWithMassEraseAndBlankCheck(UINT8 * FileDataStart)
{

	int Temp;
	uint8 command[4];
	BDM08 bdm;
	//first try to force active debug mode
	for(int i = 0; i < 5; i++)
		BdmTest(bdm);
	if(BDMExitFlag == FALSE)
	{
		pprintf("\n Sync = %d", bdm.cycleCount);
		command[0] = 0xC4;
		command[1] = 0xC8;
		bdm.Transfer(command, 2, 0, 0);
		CPU::DelayMs(10);
		//cross check if active debug mode is achieved. if not then try again.
		if((bdm.ReadSCR() & 0xC8) != 0xC8)
		{
			bdm.Send(0x00);
			bdm.Send(0x72);
			bdm.Send(0x00);
			bdm.Send(0x00);
			bdm.Send(0x31);
			bdm.Send(0x20);
			bdm.Send(0x79);
			bdm.Send(0x20);
			bdm.Send(0x00);
			bdm.Send(0x1C);
			bdm.Background();
			bdm.Send(0x00);
			bdm.Send(0x1C);
			bdm.WriteA(0x93);
			CPU::DelayMs(20);
			bdm.Send(0x92);
			bdm.Send(0x20);
			bdm.Send(0x01);
			bdm.Send(0x21);
			bdm.Send(0xC9);
			bdm.Background();
			bdm.Send(0x00);
			CPU::DelayMs(10);
		}
		printf("\n Bkpt read %04X", bdm.ReadBkpt());
		bdm.WriteBkpt(0x1234);
		printf("\n Bkpt read %04X", bdm.ReadBkpt());
		bdm.WriteBkpt(0x0000);
		pprintf("\n SCR now is %02X", bdm.ReadSCR());
		//////Some basic register read/////////////
		Temp = bdm.ReadByte(FLASH_OPTION_REG);
		pprintf("\n FOPT = %02X", Temp);
		Temp = bdm.ReadByte(FLASH_PROTECTION_REG);
		pprintf("\n FPROT = %02X", Temp);
		Temp = bdm.ReadByte(FLASH_CLOCK_DIVIDER_REG);
		pprintf("\n FCDIV = %02X", Temp);

		///// Mass Erase///////
		bdm.WriteByte(FLASH_PROTECTION_REG, 0xFF);
		Temp = bdm.ReadByte(FLASH_STATUS_REG);
		pprintf("\n FSTAT = %02X", Temp);
		bdm.WriteByte(FLASH_CLOCK_DIVIDER_REG, 0x11);
		bdm.WriteByte(0x8000, 0xFF);
		bdm.WriteByte(FLASH_COMMAND_REG, 0x41);
		bdm.WriteByte(FLASH_STATUS_REG, 0x80);

		for(int i = 0; i < 160; i++)
		{
			Temp = bdm.ReadByte(FLASH_STATUS_REG);
			if(Temp == 0xC0)
				break;
		}
		Temp = bdm.ReadByte(FLASH_STATUS_REG);
		if(Temp != 0xC0)
			pprintf("\n FSTAT After = %02X", Temp);
		else
			pprintf("\n FSTAT is all OK");

		///// Blank Check///////
		bdm.WriteByte(FLASH_CLOCK_DIVIDER_REG, 0x11);
		bdm.WriteByte(0x8000, 0xFF);
		bdm.WriteByte(FLASH_COMMAND_REG, 0x05);
		bdm.WriteByte(FLASH_STATUS_REG, 0x80);

		for(int i = 0; i < 160; i++)
		{
			Temp = bdm.ReadByte(FLASH_STATUS_REG);
			if(Temp == 0xC4)
				break;
		}
		Temp = bdm.ReadByte(FLASH_STATUS_REG);
		if(Temp != 0xC4)
			pprintf("\n FSTAT After blank check = %02X", Temp);
		else
			pprintf("\n Flash is blank");


		if(FileDataStart)
		{
			bdm.WriteByte(FLASH_CLOCK_DIVIDER_REG, 0x11);
			bdm.diagnostics = false;
			DelayVal = 200;
			//TODO: program important registers first. FTRIM first and then FOPT
			//////////////////////////////
			for(int i = 0; i < (32 * 1024) && (BDMExitFlag == FALSE) ; i++)
			{
				if(FileDataStart[i] != 0xFF)
				{
					bdm.WriteByte(0x8000 + i, FileDataStart[i]);
					bdm.WriteByte(FLASH_COMMAND_REG, 0x20);
					bdm.WriteByte(FLASH_STATUS_REG, 0x80);
					Temp = bdm.ReadByte(FLASH_STATUS_REG);
					for(int Retry = 0; (Temp != 0xC0) && (Retry < 3); Retry++)
						Temp = bdm.ReadByte(FLASH_STATUS_REG);
					if(Temp != 0xC0)
					{
						pprintf("\n Failed address %04X", 0x8000 + i);
						BDMExitFlag = TRUE;
					}
					else
					{
						pprintf(".");
					}
				}
			}
			//verify next
			for(int i = 0; i < (32 * 1024) && (BDMExitFlag == FALSE) ; i++)
			{
				if(FileDataStart[i] != 0xFF)
				{
					if(bdm.ReadByte(0x8000 + i) != FileDataStart[i])
					{
						pprintf("\n Failed at address %04X", 0x8000 + i);
						BDMExitFlag = TRUE;
					}
					else
					{
						pprintf(".");
					}
				}
			}
			if(BDMExitFlag == FALSE)//All done with no error
			{
				///////Read some basic registers to cross check//////
				Temp = bdm.ReadByte(FLASH_PROTECTION_REG);
				pprintf("\n FPROT = %02X", Temp);
				Temp = bdm.ReadByte(FLASH_CLOCK_DIVIDER_REG);
				pprintf("\n FCDIV = %02X", Temp);
				Temp = bdm.ReadByte(FLASH_OPTION_REG);
				pprintf("\n FOPT = %02X", Temp);
				///////////////////////////////
				bdm.WriteByte(0x1801, 0x01);//write SBDFR to reset the board
				bdm.SetBkgdOut();
				bdm.SetBkgd(1); //reset in user mode
			}
		}
	}
}

/*
 * void BdmTest(BDM08 & bdm1)
 *
 * Purpose:
 * 		Perform sync with target to determine the communication speed and to put the target in
 * 		Active Background mode.
 * 	Entry Condition:
 * 		LCD programming is initiated from web.
 * 	Exit Condition:
 * 		Communication speed is determined and target goes into active background mode.
 */
void BdmTest(BDM08 & bdm1)
{
	CyGOS::Stop();
	CyGOS::KickWatchDog();
	BDM08 bdm;
	bdm.SetReset(1);
	CPU::DelayMs(6);
	bdm.SetReset(0);
	CPU::DelayMs(6);
	bdm.SetReset(1);
	CPU::DelayMs(6);
	bdm.Sync();
	pprintf("\n sync = %d", bdm.cycleCount);
	if(BDMExitFlag == FALSE)
	{
		bdm.Sync();
		pprintf("\n sync = %d", bdm.cycleCount);
		bdm.Sync();
		pprintf("\n sync = %d", bdm.cycleCount);
		CPU::DelayMs(6);
		printf("\n reset status reg %02X", bdm.ReadByte(0x1800));
		uint8 command[4];
		uint8 status[2];
		command[0] = 0xE4;
		bdm.Transfer(command, 1, status, 1);
		pprintf("BDCSCR is now %02X\n", status[0]);
		CPU::DelayMs(10);
		bdm.Send(0x90);
		CPU::DelayMs(10);
		bdm.Transfer(command, 1, status, 1);
		CPU::DelayMs(10);
		pprintf("Setting BDCBKPT to 1234\n");
		command[0] = 0xC2;
		command[1] = 0x12;
		command[2] = 0x34;
		bdm.Transfer(command, 3, 0, 0);
		CPU::DelayMs(10);
		command[0] = 0xE2;
		bdm.Transfer(command, 1, status, 2);
		pprintf("BDCBKPT is now %02X%02X\n", status[0], status[1]);
		pprintf("Setting BDCBKPT to 2345\n");
		command[0] = 0xC2;
		command[1] = 0x23;
		command[2] = 0x45;
		bdm.Transfer(command, 3, 0, 0);
		CPU::DelayMs(10);
		command[0] = 0xE2;
		bdm.Transfer(command, 1, status, 2);
		pprintf("BDCBKPT is now %02X%02X\n", status[0], status[1]);
		CPU::DelayMs(10);
		pprintf("\n BDCSCR %02X ", bdm.ReadSCR());
		pprintf("Setting BDCSCR to C8\n");
		command[0] = 0xC4;
		command[1] = 0xC8;
		bdm.Transfer(command, 2, 0, 0);
		CPU::DelayMs(10);
		command[0] = 0xE4;
		bdm.Transfer(command, 1, status, 1);
		pprintf("BDCSCR is now %02X\n", status[0]);
		CPU::DelayMs(10);
		command[0] = 0xE4;
		bdm.Transfer(command, 1, status, 1);
		pprintf("BDCSCR is now %02X\n", status[0]);
		CyGOS::KickWatchDog();
		bdm.Sync();
		printf("\n Bkpt read %04X", bdm.ReadBkpt());
		bdm.WriteBkpt(0x1234);
		printf("\n Bkpt read %04X", bdm.ReadBkpt());
		bdm.WriteBkpt(0x0000);
		pprintf("\n SCR now is %02X", bdm.ReadSCR());
		RTOS::DelayMs(50);
	}
	else
	{
		pprintf("\n sync failure");
	}
	bdm1 = bdm;
}

void BdmProgram(UINT8 *dataBuffer, SINT32 dataCount)
/***************************************************************************************/
/*Description:
 This function is called from SendFileToLCDA() to program the LCD*/
/***************************************************************************************/
{
	CriticalSection cs;
	SINT32 pc, sp, ccr, a, hx;
	BDM08 bdm;
	bdm.diagnostics = false;
	//	bdm.Reset();
	bdm.Sync();
	if (BDMExitFlag == FALSE) {
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
		bdm.WriteByte(0x1801, 0, 0x01);	//Force Reset to MCU
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
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		uint8 TrimReg[2] = { 0, 0 };
		bdm.ReadByte(0xFFFE, TrimReg, 2);
		bdm.WritePC(pc);
		bdm.WriteSP(sp);
		BdmTestBreakpoint(bdm, 0x0555);
		bdm.Sync();
		bdm.ReadSCR();
		pc = bdm.ReadPC();
		sp = bdm.ReadSP();
		ccr = bdm.ReadCCR();
		a = bdm.ReadA();
		hx = bdm.ReadHX();
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
		bdm.WriteByte(0x1801, 0, 0x01);	//Force Reset to MCU
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
		bdm.WriteByte(0x1801, 0, 0x01);	//Force Reset to MCU
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
		bdm.WriteByte(0x1801, 0, 0x01);	//Force Reset to MCU
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
		pprintf("PrePare Erase\n");
		static UINT8 codeA[] = { 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x93,
		      0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80,
		      0x00, 0x00, 0x00, 0x60, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x93, 0x00,
		      0x00, 0x04, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x93, 0x00, 0x00, 0x00,
		      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		      0x40, 0x93, 0x00, 0x00, 0x50, 0x93, };
		bdm.WriteNext(0x006A, codeA, sizeof(codeA));
		bdm.ReadNext(0x7A, 0, 0x0F); // READ the RAM
		bdm.ReadNext(0x8A, 0, 0x0F); // READ the RAM
		bdm.ReadNext(0x9A, 0, 0x0F); // READ the RAM
		bdm.ReadNext(0xAA, 0, 0x0F); // READ the RAM
		bdm.ReadByte(0x006A, 0, 0x50); // READ the RAM
		CyGOS::KickWatchDog();

		pprintf("Erasing?\n");
		static UINT8 codeB[] = { 0xC6, 0x18, 0x25, 0xA4, 0x30, 0x27, 0x0A, 0xA6,
		      0x30, 0xC7, 0x18, 0x25, 0xA6, 0x30, 0xC7, 0x18, 0x25, 0xB6, 0x66,
		      0xF7, 0xB6, 0x67, 0xC7, 0x18, 0x26, 0xA6, 0x80, 0xC7, 0x18, 0x25,
		      0x81, 0x87, 0xC6, 0x18, 0x25, 0xA4, 0x40, 0x27, 0xF9, 0x86, 0x81,
		      0x45, 0x08, 0x1F, 0x94, 0x87, 0xC6, 0x18, 0x25, 0xAA, 0x30, 0xC7,
		      0x18, 0x25, 0x86, 0xC7, 0x18, 0x20, 0x45, 0x00, 0x00, 0x82 };
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
		for (LoopCount = 0; LoopCount < COUNT_MAX; LoopCount++) {
			CPU::DelayMs(10);
			SINT32 scr = bdm.ReadSCR();
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
		pprintf("Erased?\n");

		static UINT8 code2[] = { 0x45, 0x80, 0x00, 0xA6, 0xFF, 0xB7, 0x66, 0xA6,
		      0x41, 0xB7, 0x67, 0xBD, 0x6A, 0x45, 0x80, 0x00, 0xA6, 0xFF, 0xB7,
		      0x66, 0xA6, 0x05, 0xB7, 0x67, 0xBD, 0x6A, 0xBD, 0x89, 0xA6, 0x20,
		      0xB7, 0x67, 0xA6, 0xFE, 0xB7, 0x66, 0x45, 0xFF, 0xBF, 0xBD, 0x6A,
		      0xBD, 0x89, 0xCC, 0x00, 0xC1, 0xA6, 0xFF, 0x45, 0x80, 0x00, 0x71,
		      0x03, 0xCC, 0x00, 0xE0, 0x65, 0xFF, 0xBF, 0x26, 0xF6, 0x45, 0xFF,
		      0xC0, 0x71, 0x03, 0xCC, 0x00, 0xE0, 0x65, 0x00, 0x00, 0x26, 0xF6,
		      0x45, 0x00, 0x00, 0x35, 0x64, 0x82 };
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
		for (LoopCount = 0; LoopCount < COUNT_MAX; LoopCount++) {
			CPU::DelayMs(10);
			SINT32 scr = bdm.ReadSCR();
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

		bdm.PrepareProgram();
		CyGOS::KickWatchDog();
		SINT32 i;
		SINT32 DestAddr = 0x8000;
		static UINT8* LocAddress = dataBuffer, *ReadBuffer = dataBuffer;
		SINT32 j;
		UINT8 Check = 0xFF;
		for (i = 0; (i < (dataCount / 64)) && (BDMExitFlag == FALSE); i++) {
			CyGOS::KickWatchDog();

			Check = 0xFF;			//Don't Program the empty blocks
			for (j = 0; j < 64; j++)	//0x40 bytes or 64bytes written at once
			      {
				Check &= *(LocAddress++);
			}
			if (Check != 0xFF) {
				printf(".");
				bdm.ProgramFlash(DestAddr, dataBuffer, 64);
			} else
				printf("Block Empty\n");
			DestAddr += 64; //0x40
			dataBuffer += 64; //0x40
			CPU::DelayMs(10);
			CyGOS::KickWatchDog();
		}
		Check = 0xFF;
		DestAddr = 0x8000;
		LocAddress = dataBuffer = ReadBuffer;

		printf("Verify \n");
		for (i = 0; (i < (dataCount / 64)) && (BDMExitFlag == FALSE); i++) {
			CyGOS::KickWatchDog();

			Check = 0xFF;			//Don't Program the empty blocks
			for (j = 0; j < 64; j++)	//0x40 bytes or 64bytes written at once
			      {
				Check &= *(LocAddress++);
			}
			if (Check != 0xFF) {
				//Read And Verify
				printf("#");
				BDMExitFlag = FALSE;
				bdm.VerifyHX(DestAddr, dataBuffer, 64);
				if (BDMExitFlag == TRUE) {
					pprintf("FAIL to CHECK");
					printf("\n Data to Write  = %02X Data Read %02X  Address %04X\n",
					      WriteData, ReadData, Address);
				}
			} else
				printf("Block Empty\n");

			DestAddr += 64; //0x40
			dataBuffer += 64; //0x40
			CPU::DelayMs(10);
			CyGOS::KickWatchDog();
		}
		if (BDMExitFlag == FALSE) {
			printf("L0");
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
			for (LoopCount = 0; LoopCount < COUNT_MAX; LoopCount++) {
				CPU::DelayMs(10);
				SINT32 scr = bdm.ReadSCR();
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
			bdm.ReadNext(0xFBFF, 0, 0x210);
			bdm.ReadNext(0xFE0F, 0, 0x1F0);
			bdm.ReadSCR();
			CyGOS::KickWatchDog();

			bdm.WriteByte(0x0092, 0, 0x9D); //
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
			for (LoopCount = 0; LoopCount < COUNT_MAX; LoopCount++) {
				CPU::DelayMs(10);
				SINT32 scr = bdm.ReadSCR();
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
			bdm.WriteByte(0x0092, 0, 0x01); //
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
			for (LoopCount = 0; LoopCount < COUNT_MAX; LoopCount++) {
				CPU::DelayMs(10);
				SINT32 scr = bdm.ReadSCR();
				if (scr == 0xC8) {
					BDMExitFlag = FALSE;
					break;
				}
			}
			CyGOS::KickWatchDog();
			printf("L5");
			bdm.ReadSCR();
			bdm.ReadSCR();
			bdm.ReadSCR();
			bdm.ReadSCR();
			bdm.Sync();
			BdmTestBreakpoint(bdm, 0x1234);
			bdm.ReadSCR();
			bdm.WriteByte(0x1801, 0, 0x01);	//Force Reset to MCU
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
	}
}

void WriteSomeMoreData(BDM08 bdm)
/***************************************************************************************/
/*Description:
 This function is called from BDMProgram to program the LCD.*/
/***************************************************************************************/
{
	printf("z");
	BDMExitFlag = TRUE;
	for (LoopCount = 0; LoopCount < COUNT_MAX; LoopCount++) {
		CyGOS::KickWatchDog();
		SINT32 scr = bdm.ReadSCR();
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
	for (LoopCount = 0; LoopCount < COUNT_MAX; LoopCount++) {
		CPU::DelayMs(10);
		SINT32 scr = bdm.ReadSCR();
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
	for (LoopCount = 0; LoopCount < COUNT_MAX; LoopCount++) {
		CPU::DelayMs(10);
		SINT32 scr = bdm.ReadSCR();
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
	for (LoopCount = 0; LoopCount < COUNT_MAX; LoopCount++) {
		CPU::DelayMs(10);
		SINT32 scr = bdm.ReadSCR();
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
	for (LoopCount = 0; LoopCount < COUNT_MAX; LoopCount++) {
		CPU::DelayMs(10);
		SINT32 scr = bdm.ReadSCR();
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

