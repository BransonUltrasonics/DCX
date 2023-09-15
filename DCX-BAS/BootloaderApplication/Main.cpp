// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/BootloaderApplication/Main.cpp_v   1.2   06 Nov 2013 06:22:08   rjamloki  $
//
// Copyright (c) Cyber Group, Inc 2008
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This is the main module for MultiBoot loader.

#include "CyGOS.h"
#include "LED.h"
#include "DebugNew.h"
#include "stdio.h"
#include "Board.h"
#include "CpuDefs.h"
#include "CPU.h"
#include "SerialBootTask.h"

bool FramCsDone = FALSE;
void PowerUpInitialization(void);
unsigned int CalculateFlashCRC(void);
void UpdateBootLoaderInfo(void);
/*******************************************************/
#define FRAM_BASEADDRESS            0x0B000000 //(flexbus chip select 1)
//Chip select pin number on CPU
#define CHIPSELCT_PIN1 1
//wait states for memory mapped devices
//for flexbus interface initialization
#define WAITSTATES_FRAM_CS1        8
//databus width for memory mapped device
#define DATABUSWIDTH_FRAM_CS1       8

//Bootloader version start offset from the FRAM base address
#define BOOTLOADERVERSIONOFFSET_FRAM  4
//Bootloader CRC start offset from the FRAM base address
#define BOOTLOADERCRCOFFSET_FRAM    12
#define BOOTSWVERSION_LEN    8

unsigned char BootLoaderVersion[BOOTSWVERSION_LEN] = "BL1.10";
void SetLeds(int slow, int fast) {
	LED::Set(1, slow);
	LED::Set(2, fast);
}

void UpdateTPVal(int) {
}
void UpdateITPVal(int) {
}
//int currentTime11;
unsigned int CrashBuffer[4131];
char TaskName[24];
unsigned int CrashValid;
unsigned int CurrStackPtr;
unsigned int Stackbegin;
unsigned int StackEnd;
unsigned int CrashBuffPrintLen;
unsigned int MMUAR;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  Lookup Tables.  The "hardware" method of computing CRCs involves bit   */
/*  manipulations, which is very inefficient for a software computation.   */
/*  Instead of computing the CRC bit-by-bit, a 256-element lookup table    */
/*  can be used to perform the equivalent of 8 bit operations at a time.   */
/*  (This is described in "Byte-wise CRC Calculations" in IEEE Micro,      */
/*  June 1983, pp. 40-50.)  For a CRC-16, the lookup table consists of 256 */
/*  2-byte WORDs (see below, or the CRC16.PAS unit for the actual table,   */
/*  or the CRCTable program for computation of the lookup table for the    */
/*  x16 + x15 + x2 + 1 generator polynomial):                              */
/*                                                                         */
/*-------------------------------------------------------------------------*/
const unsigned short CRCTable[256] = { 0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301,
		0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1,
		0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81,
		0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
		0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00,
		0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1,
		0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641, 0xD201, 0x12C0, 0x1380,
		0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141,
		0x3300, 0xF3C1, 0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501,
		0x35C0, 0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0,
		0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881,
		0x3840, 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
		0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401,
		0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1,
		0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041, 0xA001, 0x60C0, 0x6180,
		0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740,
		0xA501, 0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01,
		0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1,
		0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80,
		0xBA41, 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
		0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200,
		0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1,
		0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780,
		0x9741, 0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41,
		0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901,
		0x59C0, 0x5880, 0x9841, 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1,
		0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80,
		0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
		0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 };

unsigned short ByteCRC(unsigned char * Ptr, unsigned int Length)
/*****************************************************************************/
/*                                                                           */
/*   The following is a little cryptic (but executes very quickly).          */
/*   The algorithm is as follows:                                            */
/*   1. exclusive-or the input byte with the low-order byte of               */
/*      the CRC register to get an INDEX                                     */
/*   2. shift the CRC register eight bits to the right                       */
/*   3. exclusive-or the CRC register with the contents of                   */
/*      Table[INDEX]                                                         */
/*   4. repeat steps 1 through 3 for all bytes}                              */
/*                                                                           */
/*****************************************************************************/
{
	unsigned short CRCRegister = 0;
	unsigned char Index;
	while (Length-- > 0) {
		Index = (unsigned char) ((CRCRegister ^ *Ptr++) & 0xff);
		CRCRegister = CRCRegister >> 8;
		CRCRegister = CRCRegister ^ CRCTable[Index];
	}
	return (CRCRegister);
}

/*  unsigned int UpdateFlashCRC(void)
 *
 *  Purpose:
 *      This function calculates the CRC of bootloader code in flash.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    returns the calculated CRC.
 */
unsigned int CalculateFlashCRC(void) {
	unsigned int FlashCRC = 0;
	int CodeSize = (int) __TEXT_END - (int) __TEXT_START;
	int FlashOffset = (int) __TEXT_START - (int) __FLASH_RAM;
	unsigned char * CodeImage = new unsigned char[CodeSize];

	memcpy((void *) CodeImage,
			(unsigned int *) (__FLASH_ROM + FlashOffset), CodeSize);
	//TODO:Need to give try by directly sending flash pointer
	FlashCRC = ByteCRC(CodeImage, (unsigned int) CodeSize);
	printf("\n BootLoader Code CRC is %08X", FlashCRC);
	delete CodeImage;
	return FlashCRC;
}

/*  void PowerUpInitialization(void)
 *
 *  Purpose:
 *    This function is called once at power up from main()function.
 *    It does call the functions to setup flex bus interface to access memory mapped devices connected to WC CPU.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
void PowerUpInitialization(void) {
	//Setup flexbus interface for FRAM
	CPU::SetupChipSelect(CHIPSELCT_PIN1, WAITSTATES_FRAM_CS1,
			DATABUSWIDTH_FRAM_CS1, (void *) FRAM_BASEADDRESS);
	FramCsDone = true;
	UpdateBootLoaderInfo();

}

/*   void UpdateBootLoaderInfo(void)
 *
 *   Purpose:
 *    This function writes version string and CRC of Bootloader code
 *    to predefined FRAM locations.
 *    This function is called once from PowerUpInitialization() function.
 *
 *   Entry condition:
 *    None.
 *
 *   Exit condition:
 *    None.
 */
void UpdateBootLoaderInfo(void) {
	unsigned char * FramBase = (unsigned char *) FRAM_BASEADDRESS;
	unsigned int FlashCRC = CalculateFlashCRC();
	memcpy(FramBase + BOOTLOADERCRCOFFSET_FRAM, &FlashCRC, sizeof(FlashCRC));
	memcpy(FramBase + BOOTLOADERVERSIONOFFSET_FRAM, BootLoaderVersion,
			BOOTSWVERSION_LEN);
}

/*   int main()
 *
 *   Purpose:
 *		This is the entry point of WC Application code. This function is called by startup assembly routines.
 *
 *   Entry condition:
 *		None.
 *
 *   Exit condition:
 *		Never returns.
 */
int main() {
	printf("\x0C \x0C \x0C BoolLoader Version %s\n", BootLoaderVersion);
	int Priority = 20;
	CyGOS::Init(); // for basic tasks initialization
	//CyGOS::CreateLedTasks(1, 1, SetLeds);
    PowerUpInitialization();
	CyGOS::CreateSerialBootTask(Priority);
	CyGOS::Start(10000); // to start CyGOS with tick value 10000 us(fix for now).
}
