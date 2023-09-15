/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/UploadFirmWareTask.cpp_v   1.1   01 Sep 2011 13:06:18   ygupta  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/UploadFirmWareTask.cpp_v  $
 * 
 *    Rev 1.1   01 Sep 2011 13:06:18   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.0   02 Mar 2011 08:14:44   PDwivedi
 * Initial revision.
 */
#include "UploadFirmWareTask.h"
#include "stdio.h"
#include "Board.h"
#include "CPU.h"
#include "DebugNew.h"
#include "ASSERT.h"
#include "Flash.h"
#include "CyGOS.h"
#include "LOG.h"
#include "Globals.h"


#define UPLOADFIRMFARETASK_STACKSIZE 512
UploadFirmware * UploadFirmware::thisPtr;

UploadFirmware::UploadFirmware():Task("UploadFirmware", UPLOADFIRMFARETASK_STACKSIZE), upLoadQ(1, "uploadQ")
{
	thisPtr = this;
}

/*This function waits indefinitely for upload command
 * to be received from website. It checs which type of
 * file to be programmed into flash and then call the
 * function to program the specific flash area.
 */
void UploadFirmware::Run()
{
	MsgUploadFirmware tempQ;
	for(;;)
	{
		upLoadQ.Read(this, tempQ, -1);
		pprintf(" upload msg type %d", tempQ.fileType);
		switch(tempQ.fileType)
		{
		case fileTypeBin:
			ProgramFlash(tempQ.fileData, tempQ.fileLen);
			break;
		case fileTypeCyg:
			ProgramCompressedTarRegionFlash(tempQ.fileData , tempQ.fileLen);
			break;
		default:
			if(tempQ.fileData)
				delete tempQ.fileData;
		}

	}
}

/*This function programs the application
 * area of flash with the received data
 * @param data:Data to be written on flash
 * @param dataLen: Length of data
 */
void UploadFirmware::ProgramFlash(char * data, int dataLen)
{
	pprintf("\n %s", __FUNCTION__);

	Flash * flash = Board::GetFlash();
	ASSERT(flash);
	pprintf("Length %d\n", dataLen);
	if (dataLen > 0)
	{
		pprintf("Programming Address %08X\n", __FLASH_ROM);
		pprintf("Flash %08X\n", flash);
		flash->Unlock(__FLASH_ROM, dataLen);
		flash->Erase(__FLASH_ROM, dataLen, EraseCallback);
		flash->Program(__FLASH_ROM, data, dataLen, ProgramCallback);
		flash->Lock(__FLASH_ROM, dataLen);
//		pprintf("Programmed, restarting...\n");
		DelayMs(1000);
//		CPU::Restart();
		Log::WriteEventLog(PROGRAM_FLASH, (unsigned short)(FLASHCODE_PROGARM) , 0, 0, false);
		pprintf("\n Waiting for Restart after flash program...\n ");
		CyGOS::DisableWatchDog();
		CyGOS::Stop();
		while(1);
	}
	delete flash;
	delete data;
}

/*This function programs the compressed TAR
 * area of flash with the received data
 * @param data:Data to be written on flash
 * @param dataLen: Length of data
 */
void UploadFirmware::ProgramCompressedTarRegionFlash(char * data, int dataLen)
{
	pprintf("\n %s", __FUNCTION__);
	Flash * flash = Board::GetFlash();
	ASSERT(flash);

	pprintf("Length %d\n", dataLen);
	if (dataLen > 0)
	{
		pprintf("Programming Address %08X\n", __TAR_START);
		pprintf("Flash %08X\n", flash);
		flash->Unlock(__TAR_START, dataLen);
		flash->Erase(__TAR_START, dataLen, EraseCallback);
		flash->Program(__TAR_START, data, dataLen, ProgramCallback);
		flash->Lock(__TAR_START, dataLen);
//		pprintf("Programmed, restarting...\n");
		DelayMs(3000);
//		CPU::Restart();
		Log::WriteEventLog(PROGRAM_FLASH, (unsigned short)(FLASHTAR_PROGRAM) , 0, 0, false);
		pprintf("\n Waiting for Restart after flash program...\n ");
		CyGOS::DisableWatchDog();
		CyGOS::Stop();
		while(1);
	}
	delete flash;
	delete data;
}

/*This function gets called by website module
 * after receiving the file to be programmed
 * on flash.
 * @param fType:type of file(bin or cyg)
 * @param data: data to be programmed
 * @param dataLen:Length of data
 */
bool UploadFirmware::ProgramFirmware(FileType fType, char * data, int dataLen)
{
	bool retVal;
	pprintf("\n %s %d", __FUNCTION__, fType);
	msg.fileType = fType;
	msg.fileLen = dataLen;
	msg.fileData = data;
	retVal = upLoadQ.Write(0, msg ,0);
	if(!retVal)
		delete data;

	return retVal;
}

