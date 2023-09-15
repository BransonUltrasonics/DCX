/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/UploadFirmWareTask.cpp_v   1.1.2.0.1.0   17 Mar 2016 16:23:46   hasanchez  $
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
 *    Rev 1.1.2.0.1.0   17 Mar 2016 16:23:46   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.1.2.1   17 Mar 2016 16:07:26   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.1.2.0   19 Sep 2014 07:35:26   amaurya
 * Changes for complete.bin upload through website.
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
#include "FUNC.h"


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
	int Sent;
	pprintf("\n %s ********************", __FUNCTION__);
	unsigned long Checksum = 0;			//added
	for(;;)
	{
		upLoadQ.Read(this, tempQ, -1);
		pprintf(" upload msg type %d", tempQ.fileType);
		pprintf("\n CRC=%d", ByteCRC((UINT8 *)tempQ.fileData,tempQ.fileLen));
		switch(tempQ.fileType)
		{
			case fileTypeBin:
				memcpy(&Checksum, tempQ.fileData, 4);
				pprintf("checksum is =%08x length = %d", Checksum, tempQ.fileLen);
				if(Checksum == CHECKSUM_COMPLETEDCP){
					printf("complete dcp function %08x\n Len %d", Checksum, (tempQ.fileLen - 4));
					ProgramFlash(tempQ.fileData, (tempQ.fileLen - 4), FlashAreaComplete);//function to be called for dcp file
				}
				else if(Checksum == CHECKSUM_APPDCP){
					printf("APP function %08x len %d\n", Checksum, (tempQ.fileLen - 4));
					ProgramFlash(tempQ.fileData, (tempQ.fileLen - 4), FlashAreaApp);//function to be called for dcp file
				}
				else{
					pprintf("NOT A DCP FILE");
					if(tempQ.fileData)
						delete tempQ.fileData;
				}
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


/*   void UploadFirmware::ProgramFlash(SINT8 * Data, SINT32 DataLen, FlashArea Flasharea)
 *
 *   Purpose:
 *   	This function programs the area of flash requested with received data
 *   	This function is called from the Run() function of this class.
 *
 *   Entry condition :
 *   	Data - Pointer to the data to be programmed on FLASH
 *   	DataLen - length of data to be programmed.
 *    Flasharea- Area of flash to be programmed bootloader, Application , cyg or complete
 *   Exit condition
 *   	None.
 */
void UploadFirmware::ProgramFlash(SINT8 * Data, SINT32 DataLen, FlashArea Flasharea)
{
	pprintf("\n %s  **** %d", __FUNCTION__, DataLen);
	SINT32 DataLenLimit = 0;
	BOOLEAN ValidFlashArea = TRUE;
	void * FlashAreaStartAddr;
	Flash * FlashPtr = Board::GetFlash();
	SINT8 * FlashData = Data;
	ASSERT(FlashPtr);

   if(Flasharea == FlashAreaApp){
      FlashData += 4;
      DataLenLimit = (SINT32)__FLASH_SIZE;
      FlashAreaStartAddr = (void *)__FLASH_ROM;
   }
   else if(Flasharea == FlashAreaComplete){
      FlashData += 4;
      DataLenLimit = (SINT32)__FLASH_SIZE + (SINT32)__BOOTLOADER_SIZE + (SINT32)__TAR_SIZE;
      FlashAreaStartAddr = (void *)0;
   }
   else
      ValidFlashArea = FALSE;
   pprintf("Length %d\n", DataLen);
	if(ValidFlashArea == TRUE){
      if((DataLen > 0) && (DataLen <= DataLenLimit)){
         pprintf("Programming Address %08X\n", FlashAreaStartAddr);
         pprintf("Flash %08X Datalen %d\n", FlashPtr, DataLen);
         FlashPtr -> Unlock(FlashAreaStartAddr, DataLen);
         FlashPtr -> Erase(FlashAreaStartAddr, DataLen,EraseCallback);
         FlashPtr -> Program(FlashAreaStartAddr, FlashData, DataLen, ProgramCallback);
         FlashPtr -> Lock(FlashAreaStartAddr, DataLen);
		DelayMs(1000);
		Log::WriteEventLog(PROGRAM_FLASH, (unsigned short)(FLASHCODE_PROGARM) , 0, 0, false);

		CPU::Restart();
      pprintf("\n Waiting for Restart after flash program...\n ");
		CyGOS::Stop();
         while(1);
      	/*pprintf("Programming Address %08X\n", __FLASH_ROM);
			pprintf("Flash %08X\n", FlashPtr);
			FlashPtr->Unlock(__FLASH_ROM, DataLen);
			FlashPtr->Erase(__FLASH_ROM, DataLen, EraseCallback);
			FlashPtr->Program(__FLASH_ROM, Data, DataLen, ProgramCallback);
			FlashPtr->Lock(__FLASH_ROM, DataLen);
	//		pprintf("Programmed, restarting...\n");
			DelayMs(1000);
			CPU::Restart();
			Log::WriteEventLog(PROGRAM_FLASH, (unsigned short)(FLASHCODE_PROGARM) , 0, 0, false);
			pprintf("\n Waiting for Restart after flash program...\n ");
			CyGOS::DisableWatchDog();
			CyGOS::Stop();
			while(1);*/
      }
	}
	CyGOS::StartTimer(1);
	delete FlashPtr;
	delete Data;
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
bool UploadFirmware::ProgramFirmware(FileType fType, char * data, int dataLen,SINT8 * Action)
{
	bool retVal;
	pprintf("\n %s %d %s", __FUNCTION__, fType,Action);
	msg.fileType = fType;
	msg.fileLen = dataLen;
	msg.fileData = data;
	msg.Action = Action;
	retVal = upLoadQ.Write(0, msg ,0);
	if(!retVal)
		delete data;
	return retVal;
}

