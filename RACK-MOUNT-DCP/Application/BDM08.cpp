/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BDM08.cpp_v   1.6   21 Sep 2015 06:44:48   RJamloki  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BDM08.cpp_v  $
 * 
 *    Rev 1.6   21 Sep 2015 06:44:48   RJamloki
 * Added InitBDMPins() to configures CPU pins required for LCD programming.
 * 
 *    Rev 1.5   19 Mar 2015 06:27:18   sesharma
 * Branson Coding Standard Applied.
 * 
 *    Rev 1.4   28 Jan 2015 06:30:38   sesharma
 * Redefined Printf to Remove Compiler Warnings
 * 
 *    Rev 1.3   07 Nov 2014 15:24:30   ewomack
 * Enable printing
 * 
 *    Rev 1.2   17 Sep 2014 08:41:20   amaurya
 * Added Comments, code review changes fixes.
 * 
 *    Rev 1.1   03 Sep 2014 07:35:24   amaurya
 * Removed printf.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:20   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:56   amaurya
 * Initial revision.
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
//#define printf
//#define pprintf

UINT32 LoopCount = 0;
UINT8 BDMExitFlag = FALSE;
UINT8 WriteData, ReadData;
SINT32 Address;
int DelayVal = 1000;//Delay in us after completing one BDM command. Its taken in
					//a variable so that it can be changed.

BDM08::BDM08()
/***************************************************************************************/
/*Description:
 BDM08 constructor called from BdmProgram. Configuring EPORT data direction register
 */
/***************************************************************************************/
{
	InitBDMPins();
	diagnostics = false;
	SetBkgdOut();
	SetBkgd(1);
	SetReset(1);
	cycleCount = 0;
}

BDM08::~BDM08() {
}

void PrintF(...)
{

}

void BDM08::SyncCycle(SINT32 WaitCount)
/****************************************************************************/
/*
 * Sync: Reset the target CPU and pulse BDM low for 10ms. This will cause
 * target CPU to pulse BDM low as soon as it is set to high by the host CPU.
 * The low pulse length from the target is 128 BDM cycles long.
 * For Platform1A board using one LCD board BDM pulse was 124-128 counts long,
 * and over 98% of values were 127 or 128 counts.
 */
/******************************************************************************/
{
	CyGOS::KickWatchDog();
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
		if(LoopCount >= WaitCount)
			BDMExitFlag = TRUE;
	}
	cycleCount = 0;
	if(BDMExitFlag == FALSE) {
		while (GetBkgd() == 0){
			cycleCount++;
		}
		if (diagnostics){
			printf("Sync: %d\n", cycleCount);
		}
	}
	SetBkgd(1);
	SetBkgdOut();
	CPU::DelayMs(5);
	CyGOS::KickWatchDog();

}

void BDM08::Sync(void)
/*******************************************************************************************/
/*Description:
 This function establishes the clock rate of the target to initiate the communication
 with LCD.*/
/*******************************************************************************************/
{
	UINT8 Count = 0;
	CyGOS::KickWatchDog();

	SyncCycle(LOOP_MAX); 
	if(cycleCount > MAX_COUNT)
		SyncCycle(LOOP2_MAX);

	if(BDMExitFlag == TRUE)
		{
		    for(Count = 0; Count < 20; Count++){
		    	CPU::DelayMs(50);
		    }
			ReadSCR();
			ReadSCR();
			ReadSCR();
			SyncCycle(LOOP2_MAX);
			if(BDMExitFlag == TRUE) printf("\n***************SYNC FAILED******************\n");
		}

}

void BDM08::Transfer(UINT8 * send, SINT32 sendLength, UINT8 * recv,
      SINT32 recvLength)
      /***************************************************************************************/
      /*Description:
       This function is called while sending different BDC commands like ReadPC, ReadSP,
       WriteCCR etc .*/
      /***************************************************************************************/
      {
	while (sendLength--) {
		Send(*send++);
	}
	SINT32 rc = (cycleCount + 4) / 8; // Counter for total reads per bit
	SINT32 counter = 0;
	while (GetBkgd() != 0 && counter < rc) {
		counter++;
	}
	Recv(recv, recvLength);
	CyGOS::KickWatchDog();

}

void BDM08::Recv(UINT8 * recv, SINT32 recvLength)
/********************************************************************************************/
/*Description:
 This function sets the counter for zeroes when receiving, thresholds between 0 and 1,
 counter for total reads per bit.*/
/********************************************************************************************/
{
	SINT32 tx = (cycleCount + 40) / 80; // Counter for zeroes when receiving
	SINT32 rc = (cycleCount + 4) / 8; // Counter for total reads per bit
	SINT32 bl = (cycleCount + 10) / 20; // Bit length, threshold between 0 and 1
	ASSERT(recvLength <= 2);
	SINT32 data[16];
	SINT32 bits = 8 * recvLength;
	CyGOS::KickWatchDog();

	for (SINT32 bit = 0; bit < bits; bit++) {

		SetBkgd(1);
		SetBkgdOut();
		for (SINT32 t = tx; t > 0; t--) {
			SetBkgd(0);
		}
		SetBkgdIn();
		SetBkgd(1);
		SINT32 counter = 0;
		while (GetBkgd() == 0) {
			counter++;
		}
		data[bit] = counter;
		while (GetBkgd() != 0 && counter < rc) {
			counter++;
		}
	}
	SetBkgd(1);
	SetBkgdOut();
	for (SINT32 bit = 0; bit < bits; bit++) {
		SINT32 index = bit / 8;
		if ((bit & 7) == 0)
			recv[index] = 0;
		recv[index] <<= 1;
		if (data[bit] < bl)
			recv[index] |= 1;
	}
	CyGOS::KickWatchDog();
}

void BDM08::Send(SINT32 byte)
/***************************************************************************************/
/*Description:
 This function defines the counter for zeroes and ones for sending '0', '1'
 and set Bkgd pin accordingly.*/
/***************************************************************************************/
{
	SINT32 tx00 = cycleCount / 13; // Counter for zeroes when sending '0'
	SINT32 tx01 = cycleCount / 20; // Counter for ones when sending '0'
	SINT32 tx10 = cycleCount / 40; // Counter for zeroes when sending '1'
	SINT32 tx11 = cycleCount / 10; // Counter for ones when sending '1'
	CyGOS::KickWatchDog();

	//SR sr = EnterCritical();
	SetBkgd(1);
	SetBkgdOut();
	for (SINT32 i = 0; i < 8; i++) {
		if (byte & 0x80) {
			for (SINT32 t0 = tx10; t0 > 0; t0--) {
				SetBkgd(0);
			}
			for (SINT32 t1 = tx11; t1 > 0; t1--) {
				SetBkgd(1);
			}
		} else {
			for (SINT32 t0 = tx00; t0 > 0; t0--) {
				SetBkgd(0);
			}
			for (SINT32 t1 = tx01; t1 > 0; t1--) {
				SetBkgd(1);
			}
		}
		byte <<= 1;
	}
	CyGOS::KickWatchDog();
}

SINT32 BDM08::ReadPC()
/***************************************************************************************/
/*Description:
 Reads the program counter*/
/***************************************************************************************/
{
	UINT8 send[1];
	send[0] = 0x6B;
	UINT8 recv[2];
	Transfer(send, 1, recv, 2);
	SINT32 pc = (recv[0] << 8) | recv[1];
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Read PC %04X\n", pc);
	}
	CyGOS::KickWatchDog();
	return pc;
}

void BDM08::WritePC(SINT32 pc)
/***************************************************************************************/
/*Description:
 Write the program counter. */
/***************************************************************************************/
{
	UINT8 send[3];
	send[0] = 0x4B;
	send[1] = pc >> 8;
	send[2] = pc;
	Transfer(send, 3, 0, 0);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Write PC %04X\n", pc);
	}
	CyGOS::KickWatchDog();
}

SINT32 BDM08::ReadSP()
/***************************************************************************************/
/*Description:
 Read the stack pointer */
/***************************************************************************************/
{
	UINT8 send[1];
	send[0] = 0x6F;
	UINT8 recv[2];
	Transfer(send, 1, recv, 2);
	SINT32 sp = (recv[0] << 8) | recv[1];
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Read SP %04X\n", sp);
	}
	CyGOS::KickWatchDog();
	return sp;
}

void BDM08::WriteSP(SINT32 sp)
/***************************************************************************************/
/*Description:
 Write the stack pointer */
/***************************************************************************************/
{
	UINT8 send[3];
	send[0] = 0x4F;
	send[1] = sp >> 8;
	send[2] = sp;
	Transfer(send, 3, 0, 0);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Write SP %04X\n", sp);
	}
	CyGOS::KickWatchDog();
}

SINT32 BDM08::ReadCCR()
/***************************************************************************************/
/*Description:
 Read condition code register. */
/***************************************************************************************/
{
	UINT8 send[1];
	send[0] = 0x69;
	UINT8 recv[1];
	Transfer(send, 1, recv, 1);
	SINT32 ccr = recv[0];
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Read CCR %02X\n", ccr);
	}
	return ccr;


}

void BDM08::WriteCCR(SINT32 ccr)
/***************************************************************************************/
/*Description:
 Write condition code register. */
/***************************************************************************************/
{
	UINT8 send[2];
	send[0] = 0x49;
	send[1] = ccr;
	Transfer(send, 2, 0, 0);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Write CCR %02X\n", ccr);
	}
	CyGOS::KickWatchDog();
}

SINT32 BDM08::ReadSCR()
/***************************************************************************************/
/*Description:
 Read BDC status from BDCSCR */
/***************************************************************************************/
{
	UINT8 send[1];
	send[0] = 0xE4;
	UINT8 recv[1];
	Transfer(send, 1, recv, 1);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Read SCR %02X\n", recv[0]);
	}
	CyGOS::KickWatchDog();
	return recv[0];
}

void BDM08::WriteSCR(SINT32 value)
/***************************************************************************************/
/*Description:
 Write BDC controls in BDCSCR */
/***************************************************************************************/
{
	UINT8 send[2];
	send[0] = 0xC4;
	send[1] = value;
	Transfer(send, 2, 0, 0);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Write SCR %02X\n", value);
	}
	CyGOS::KickWatchDog();

}

SINT32 BDM08::ReadA()
/***************************************************************************************/
/*Description:
 Read accumulator */
/***************************************************************************************/
{
	UINT8 send[1];
	send[0] = 0x68;
	UINT8 recv[1];
	Transfer(send, 1, recv, 1);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Read A %02X\n", recv[0]);
	}
	CyGOS::KickWatchDog();
	return recv[0];
}

void BDM08::WriteA(SINT32 a)
/***************************************************************************************/
/*Description:
 Write accumulator */
/***************************************************************************************/
{
	UINT8 send[2];
	send[0] = 0x48;
	send[1] = a;
	Transfer(send, 2, 0, 0);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Write A %02X\n", a);
	}
	CyGOS::KickWatchDog();

}

SINT32 BDM08::ReadHX()
/***************************************************************************************/
/*Description:
 Read H and X register pair (H:X) */
/***************************************************************************************/
{
	UINT8 send[1];
	send[0] = 0x6C;
	UINT8 recv[2];
	Transfer(send, 1, recv, 2);
	SINT32 hx = (recv[0] << 8) | recv[1];
	if (diagnostics) {
		pprintf("Read HX %04X\n", hx);
	}
	CPU::DelayUs(DelayVal);
	CyGOS::KickWatchDog();
	return hx;
}

void BDM08::WriteHX(SINT32 hx)
/***************************************************************************************/
/*Description:
 Write H and X register pair (H:X) */
/***************************************************************************************/
{
	UINT8 send[3];
	send[0] = 0x4C;
	send[1] = hx >> 8;
	send[2] = hx;
	Transfer(send, 3, 0, 0);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Write HX %04X\n", hx);
	}
	CyGOS::KickWatchDog();
}

void BDM08::Go()
/***************************************************************************************/
/*Description:
 Execute the user application program starting at the address currently in the PC */
/***************************************************************************************/
{
	UINT8 send[1];
	send[0] = 0x08;
	Transfer(send, 1, 0, 0);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Go\n");
	}
	CyGOS::KickWatchDog();
}

void BDM08::Background()
/***************************************************************************************/
/*Description:
 Enter active background mode if enabled. */
/***************************************************************************************/
{
	UINT8 send[1];
	send[0] = 0x90;
	Transfer(send, 1, 0, 0);
	CPU::DelayUs(DelayVal);
	CyGOS::KickWatchDog();
}

SINT32 BDM08::ReadBkpt()
/***************************************************************************************/
/*Description:
 Read BDCBKPT breakpoint register. */
/***************************************************************************************/
{
	UINT8 send[1];
	send[0] = 0xE2;
	UINT8 recv[2];
	Transfer(send, 1, recv, 2);
	CPU::DelayUs(DelayVal);
	SINT32 b = (recv[0] << 8) | recv[1];
	if (diagnostics) {
		pprintf("Read BKPT %04X\n", b);
	}
	CyGOS::KickWatchDog();
	return b;
}

void BDM08::WriteBkpt(SINT32 value)
/***************************************************************************************/
/*Description:
 Write BDCBKPT breakpoint register. */
/***************************************************************************************/
{
	UINT8 send[3];
	send[0] = 0xC2;
	send[1] = value >> 8;
	send[2] = value;
	Transfer(send, 3, 0, 0);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Write BKPT %04X\n", value);
	}
	CyGOS::KickWatchDog();
}

void BDM08::ReadByte(SINT32 address, UINT8 * data, SINT32 length)
/***************************************************************************************/
/*Description:
 Read bytes of data  from target memory depending on length. */
/***************************************************************************************/
{
	while (length--) {
		UINT8 send[3];
		send[0] = 0xE0;
		send[1] = address >> 8;
		send[2] = address;
		UINT8 recv[1];
		Transfer(send, 3, recv, 1);
		CPU::DelayUs(DelayVal);
		if (diagnostics) {
			pprintf("Read byte [%04X] data %02X\n", address, recv[0]);
		}
		if (data)
			*data++ = recv[0];
		address++;
	}
}

SINT32 BDM08::ReadByte(SINT32 address)
/***************************************************************************************/
/*Description:
 Read a byte from target memory. */
/***************************************************************************************/
{
	UINT8 send[3];
	send[0] = 0xE0;
	send[1] = address >> 8;
	send[2] = address;
	UINT8 recv[1];
	Transfer(send, 3, recv, 1);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Read byte [%04X] data %02X\n", address, recv[0]);
	}
	return recv[0];
}

void BDM08::WriteByte(SINT32 address, UINT8 * data, SINT32 length)
/***************************************************************************************/
/*Description:
 Write bytes of data to the target memory depending on length. */
/***************************************************************************************/
{
	while (length--) {
		UINT8 send[4];
		send[0] = 0xC0;
		send[1] = address >> 8;
		send[2] = address;
		send[3] = *data++;
		Transfer(send, 4, 0, 0);
		CPU::DelayUs(DelayVal);
		if (diagnostics) {
			pprintf("Write byte [%04X] data %02X\n", address, send[3]);
		}
		address++;
	}
}

void BDM08::WriteByte(SINT32 address, SINT32 data)
/***************************************************************************************/
/*Description:
 Write a byte to target memory. */
/***************************************************************************************/
{
	UINT8 send[4];
	send[0] = 0xC0;
	send[1] = address >> 8;
	send[2] = address;
	send[3] = data;
	Transfer(send, 4, 0, 0);
	CPU::DelayUs(DelayVal);
	if (diagnostics) {
		pprintf("Write byte [%04X] data %02X\n", address, data);
	}
}

void BDM08::ReadNext(SINT32 address, UINT8 * data, SINT32 length)
/***************************************************************************************/
/*Description:
 Increment H:X by one then read memory byte located at H:X. */
/***************************************************************************************/
{
	SINT32 hx = ReadHX();
	WriteHX(address - 1);
	while (length--) {
		UINT8 send[1];
		send[0] = 0x70;
		UINT8 recv[1];
		Transfer(send, 1, recv, 1);
		CPU::DelayUs(DelayVal);
		if (diagnostics) {
			pprintf("Read [+HX] data %02X\n", recv[0]);
		}
		if (data)
			*data++ = recv[0];
	}
	WriteHX(hx);
}

void BDM08::WriteNext(SINT32 address, UINT8 data) {
	WriteNext(address, &data, 1);
}

void BDM08::WriteNext(SINT32 address, UINT8 * data, SINT32 length)
/***************************************************************************************/
/*Description:
 Increment H:X by one, then write memory byte located at H:X. */
/***************************************************************************************/
{
	SINT32 hx = ReadHX();
	WriteHX(address - 1);
	while (length--) {
		UINT8 send[2];
		send[0] = 0x50;
		send[1] = *data++;
		Transfer(send, 2, 0, 0);
		CPU::DelayUs(DelayVal);
		if (diagnostics) {
			pprintf("Write [+HX] data %02X\n", send[1]);
		}
	}
	WriteHX(hx);
}

void BDM08::VerifyHXBlank(SINT32 address, SINT32 length)
/***************************************************************************************/
/*Description:
 Increment H:X by one then read memory byte located at H:X. */
/***************************************************************************************/
{
	SINT32 hx = ReadHX();
	WriteHX(address - 1);
	while (length--) {
		UINT8 send[1];
		send[0] = 0x70;
		UINT8 recv[1];
		Transfer(send, 1, recv, 1);
		CPU::DelayUs(DelayVal);
		if (diagnostics) {
			pprintf("Read [+HX] at %04X data %02X\n", address, recv[0]);
		}
		address++;
	}
	WriteHX(hx);
}

void BDM08::PrepareProgram() {
	static UINT8 program[] = { 0xA6, 0x20, 0xB7, 0x67, 0xB6, 0x64, 0xB7, 0x68,
	      0xB6, 0x65, 0xB7, 0x69, 0xA6, 0xFF, 0xC7, 0x18, 0x24, 0xA6, 0x93, 0xB0,
	      0x63, 0xC7, 0x04, 0xBC, 0xC7, 0x04, 0xE4, 0xA6, 0x00, 0xB2, 0x62, 0xC7,
	      0x04, 0xBB, 0xC7, 0x04, 0xE3, 0x55, 0x62, 0xD6, 0xFF, 0xFF, 0xB7, 0x66,
	      0xBD, 0x6A, 0xAF, 0x01, 0x8B, 0x89, 0x55, 0x64, 0xAF, 0xFF, 0x35, 0x64,
	      0x65, 0x00, 0x00, 0x88, 0x8A, 0x27, 0x04, 0xBD, 0x89, 0x20, 0xE4, 0xB6,
	      0x68, 0xB7, 0x64, 0xB6, 0x69, 0xB7, 0x65, 0xBD, 0x89, 0x55, 0x62, 0xD6,
	      0xFF, 0xFF, 0xF1, 0x26, 0x15, 0xAF, 0x01, 0x8B, 0x89, 0x55, 0x64, 0xAF,
	      0xFF, 0x35, 0x64, 0x65, 0x00, 0x00, 0x88, 0x8A, 0x26, 0xE9, 0x45, 0x00,
	      0x00, 0x82, 0x82\
 };
	pprintf("\nPrepare\n");
	WriteNext(0x0493, program, sizeof(program));
	ReadSCR();
}

void BDM08::ProgramFlash(SINT32 address, UINT8 * data, SINT32 length)
/***************************************************************************************/
/*Description:
 This function programs the address in the flash to be programed. */
/***************************************************************************************/
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
	CyGOS::KickWatchDog();
	for (LoopCount = 0; LoopCount < COUNT_MAX; LoopCount++) {
		CPU::DelayMs(10);
		SINT32 scr = ReadSCR();
		if (scr == 0xC8) {
			BDMExitFlag = FALSE;
			break;
		}
	}
	Expect(ReadPC(), 0x04FD);
	Expect(ReadSP(), 0x081E);
	Expect(ReadCCR(), 0x6A);
	Expect(ReadA(), data[length - 1]);
	Expect(ReadHX(), 0x0000);
	Expect(ReadByte(0x1821), 0xC2);
	CyGOS::KickWatchDog();
}

/*void BDM08::ProgramFlashByte(SINT32 address, SINT32 data)
 {
 UINT8 byte[1];
 byte[0] = data;
 ProgramFlash(address, byte, 1);
 }*/

/*
 void BDM08::ProgramFlashWord(SINT32 address, SINT32 data)
 {
 UINT8 byte[2];
 byte[0] = data >> 8;
 byte[1] = data >> 0;
 ProgramFlash(address, byte, 2);
 }
 */

void BDM08::Expect(SINT32 read, SINT32 expect)
/***************************************************************************************/
/*Description:
 This function is for debugging while bdm program. */
/***************************************************************************************/
{
	if (read != expect) {
		//pprintf("Read %02X, expected %02X\n", read, expect);
	}
}

/*void BDM08::PrepareCalculateChecksum()
 {
 static UINT8 codeSomething[] =
 { 0x87, 0x8B, 0x89, 0xF6, 0x45, 0x00, 0xB7, 0x87, 0xEB, 0x01, 0xE7, 0x01,
 0x4F, 0xF9, 0xF7, 0x86, 0xAE, 0x08, 0x9E, 0xE8, 0x03, 0x48, 0x24,
 0x02, 0xA8, 0x07, 0x5A, 0x26, 0xF8, 0x9E, 0xE7, 0x03, 0x88, 0x8A,
 0x65, 0xFF, 0xFF, 0xAF, 0x01, 0x26, 0xD8, 0x86, 0x82, 0x9D, 0x9D,
 0x00, 0x00, 0x00 };
 static UINT8 verifyBuffer[sizeof(codeSomething)];
 WriteByte(0x008A, codeSomething, sizeof(codeSomething));
 ReadByte(0x008A, verifyBuffer, sizeof(codeSomething));
 CyGOS::KickWatchDog();
 if (memcmp(codeSomething, verifyBuffer, sizeof(codeSomething)) != 0){
 pprintf("Verify Code for Something failed\n");
 }
 }*/

/*
 SINT32 BDM08::CalculateChecksum(SINT32 address, SINT32 length)
 {
 SINT32 end = address + length - 1;
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
 SINT32 scr = ReadSCR();
 if (scr == 0xC8) {
 BDMExitFlag = FALSE;
 break;
 }
 }
 Expect(ReadPC(), 0x00B5);
 Expect(ReadSP(), 0x081E);
 Expect(ReadCCR(), 0x6A);
 ReadA();
 Expect(ReadHX(), address + length);
 SINT32 b1 = ReadByte(0x00B7);
 SINT32 b2 = ReadByte(0x00B8);
 return (b1 << 8) | b2;
 }
 */

void BDM08::VerifyHX(SINT32 address, UINT8 * data, SINT32 length) {
	SINT32 hx = ReadHX();
	WriteHX(address - 1);
	while (length--) {
		UINT8 send[1];
		send[0] = 0x70;
		UINT8 recv[1];
		Transfer(send, 1, recv, 1);
		CPU::DelayUs(DelayVal);
		if (diagnostics) {
			pprintf("Read [+HX] data %02X\n", recv[0]);
		}
		if (recv[0] != *data++) {
			pprintf("Verify failed, address %04X data %02X\n", address, recv[0]);
			BDMExitFlag = TRUE;
			length = 0;
			ReadData = recv[0];
			WriteData = *(data - 1);
			Address = address;
		}
		address++;
	}
	WriteHX(hx);
}



/*  void BDM08::InitBDMPins(void)
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
 * 	CPU pins(L3 and M2) connected to Background Debug and reset lines of
 * 	LCD processor configured as general purpose output.
*/
void BDM08::InitBDMPins(void)
{
	MCF_EPORT_EPPAR &= 0xFF3F;
	MCF_EPORT_EPDDR |= 0x08;
	MCF_EPORT_EPIER &= ~0x08;
	MCF_PAD_PAR_UART &= 0xF3;
	MCF_GPIO_PDDR_UART |= MCF_PAD_PAR_UART_PAR_U0RTS_U0RTS;
}
