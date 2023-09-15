/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UploadFirmWareTask.cpp_v   1.10   04 Dec 2015 05:11:00   sesharma  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UploadFirmWareTask.cpp_v  $
 * 
 *    Rev 1.10   04 Dec 2015 05:11:00   sesharma
 * UploadFirmware::Run() and ProgramFirmware() modified to support LCD bin file handling.
 * 
 *    Rev 1.9   20 Nov 2015 01:58:54   sesharma
 * UploadFirmware::ProgramFirmware() modified to validate the checksum of the received binary file.
 * 
 *    Rev 1.8   28 Sep 2015 06:47:36   RJamloki
 * Removed support for LCD firmware upgrade
 * 
 *    Rev 1.7   04 Jun 2015 07:52:26   sesharma
 * Type casting for Data1 & Data2 fields changed to SINT32 when writing an event. Branson coding standard formatter applied.
 * 
 *    Rev 1.6   19 Mar 2015 07:10:30   sesharma
 * Branson Coding Standard Applied.
 * 
 *    Rev 1.5   28 Jan 2015 05:36:32   sesharma
 * Formatting done
 * 
 *    Rev 1.4   13 Jan 2015 06:02:32   sesharma
 * Latching Alarm issues resolved, LCD Registers no. modified for window limits & memory clear Registers, Mergerd Rack-S & DC-Basic Changes
 * 
 *    Rev 1.3   27 Nov 2014 09:15:16   sesharma
 * Webpages and SIG related changes 
 * 
 *    Rev 1.2   17 Sep 2014 09:03:34   amaurya
 * Added support for completebin, appbin upload from website
 * 
 *    Rev 1.1   03 Sep 2014 07:42:42   amaurya
 * Added SendLCDAdata() for LCD Code Download from website.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:30   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:06   amaurya
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

UINT32 Checksum_Start = 0;
UINT32 Checksum_End = 0;
UploadFirmware::UploadFirmware() :
      Task("UploadFirmware", UPLOADFIRMFARETASK_STACKSIZE), upLoadQ(1,
            "uploadQ")
{
   thisPtr = this;
}

void UploadFirmware::SendLCDAdata(SINT8 * Data, SINT32 DataLen)
/***************************************************************************************
 * This function is called from UploadFirmware::Run() to send
 * LCD bin file to upload from website								*/
/******************************************************************************************/
{
   FirmwareUpgradeCommand Cmd;
   Cmd.Type = FileLCDA;
   Cmd.FileLength = DataLen;
   Cmd.FileData = (unsigned char *) Data;
   FirmwareUpgradeTask::thisPtr->FUWriteQueue(Cmd);
}
void UploadFirmware::Run()
/****************************************************************************************/
/*This function waits indefinitely for upload command
 * to be received from website. It checs which type of
 * file to be programmed into flash and then call the
 * function to program the specific flash area.
 */
/***************************************************************************************/
{
   MsgUploadFirmware tempQ;
   pprintf("\n %s ********************", __FUNCTION__);
   for (;;) {
      upLoadQ.Read(this, tempQ, -1);
      pprintf(" upload msg type %d", tempQ.fileType);
      pprintf("\n CRC=%d", ByteCRC((UINT8 *) tempQ.fileData, tempQ.fileLen));
      Checksum_Start = 0;
      Checksum_End = 0;
      switch (tempQ.fileType) {
      case fileTypeBin:
         memcpy(&Checksum_Start, tempQ.fileData, 4);
         memcpy(&Checksum_End, (tempQ.fileData + tempQ.fileLen - 4), 4);
         if (Checksum_Start == CHECKSUMLCDA) {
            pprintf("\r\n LCD Bin file to be upload %d", tempQ.fileLen);
            SendLCDAdata(tempQ.fileData, tempQ.fileLen);   //function to be called for Lcd file
         }
         else if ((Checksum_Start == CHECKSUM_COMPLETEDCP)
               || (Checksum_End == CHECKSUM_COMPLETEDCP)) {
            if (Checksum_Start != CHECKSUM_COMPLETEDCP) Checksum_Start = 0;
            ProgramFlash(tempQ.fileData, (tempQ.fileLen - 4),
                  FlashAreaComplete);   //function to be called for dcp file
         }
         else if ((Checksum_Start == CHECKSUM_APPDCP)
               || (Checksum_End == CHECKSUM_APPDCP)) {
            if (Checksum_Start != CHECKSUM_APPDCP) Checksum_Start = 0;
            ProgramFlash(tempQ.fileData, (tempQ.fileLen - 4), FlashAreaApp);   //function to be called for dcp file
         }
         else {
            pprintf("NOT A DCP FILE");
            if (tempQ.fileData) delete tempQ.fileData;
         }
         break;
      case fileTypeCyg:
         ProgramCompressedTarRegionFlash(tempQ.fileData, tempQ.fileLen);
         break;
      default:
         if (tempQ.fileData) delete tempQ.fileData;
      }

   }
}

void UploadFirmware::ProgramFlash(SINT8 * Data, SINT32 DataLen,
      FlashArea Flasharea)
/*********************************************************************************************/
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
/*********************************************************************************************/
{
   pprintf("\n %s  **** %d", __FUNCTION__, DataLen);
   SINT32 DataLenLimit = 0;
   BOOLEAN ValidFlashArea = TRUE;
   void * FlashAreaStartAddr;
   Flash * FlashPtr = Board::GetFlash();
   SINT8 * FlashData = Data;
   ASSERT(FlashPtr);

   if (Checksum_Start != 0) FlashData += 4;

   if (Flasharea == FlashAreaApp) {
      DataLenLimit = (SINT32) __FLASH_SIZE;
      FlashAreaStartAddr = (void *) __FLASH_ROM;
   }
   else if (Flasharea == FlashAreaComplete) {
      DataLenLimit = (SINT32) __FLASH_SIZE + (SINT32) __BOOTLOADER_SIZE
            + (SINT32) __TAR_SIZE;
      FlashAreaStartAddr = (void *) 0;
   }
   else
      ValidFlashArea = FALSE;
   pprintf("Length %d\n", DataLen);
   if (ValidFlashArea == TRUE) {
      if ((DataLen > 0) && (DataLen <= DataLenLimit)) {
         pprintf("Programming Address %08X\n", FlashAreaStartAddr);
         pprintf("Flash %08X Datalen %d\n", FlashPtr, DataLen);
         FlashPtr->Unlock(FlashAreaStartAddr, DataLen);
         FlashPtr->Erase(FlashAreaStartAddr, DataLen, EraseCallback);
         FlashPtr->Program(FlashAreaStartAddr, FlashData, DataLen,
               ProgramCallback);
         FlashPtr->Lock(FlashAreaStartAddr, DataLen);
         DelayMs(1000);
         Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (SINT32) (FLASHCODE_PROGARM),
               0, 0, false);

         CPU::Restart();
         pprintf("\n Waiting for Restart after flash program...\n ");
         CyGOS::Stop();
         while (1)
            ;
      }
   }
   delete FlashPtr;
   delete Data;
}

void UploadFirmware::ProgramCompressedTarRegionFlash(char * data,
      SINT32 dataLen)
/********************************************************************************/
/*This function programs the compressed TAR
 * area of flash with the received data
 * @param data:Data to be written on flash
 * @param dataLen: Length of data
 */
/*********************************************************************************/
{
   pprintf("\n %s", __FUNCTION__);
   Flash * flash = Board::GetFlash();
   ASSERT(flash);

   pprintf("Length %d\n", dataLen);
   if (dataLen > 0) {
      pprintf("Programming Address %08X\n", __TAR_START);
      pprintf("Flash %08X\n", flash);
      flash->Unlock(__TAR_START, dataLen);
      flash->Erase(__TAR_START, dataLen, EraseCallback);
      flash->Program(__TAR_START, data, dataLen, ProgramCallback);
      flash->Lock(__TAR_START, dataLen);
//		pprintf("Programmed, restarting...\n");
      DelayMs(3000);
      CPU::Restart();
      Log::WriteEventLog(EVENT_PROGRAMFIRMWARE, (SINT32) (FLASHTAR_PROGRAM), 0,
            0, false);
      pprintf("\n Waiting for Restart after flash program...\n ");
      CyGOS::DisableWatchDog();
      CyGOS::Stop();
      while (1)
         ;
   }
   delete flash;
   delete data;
}
bool UploadFirmware::ProgramFirmware(FileType fType, char * data, int dataLen,
      SINT8 * Action)
/*********************************************************************************************/
/*This function gets called by website module
 * after receiving the file to be programmed
 * on flash.
 * @param fType:type of file(bin or cyg)
 * @param data: data to be programmed
 * @param dataLen:Length of data
 */
/**********************************************************************************************/
{
   bool retVal;
   pprintf("\n %s %d %s", __FUNCTION__, fType, Action);
   msg.fileType = fType;
   msg.fileLen = dataLen;
   msg.fileData = data;
   msg.Action = Action;

   memcpy(&Checksum_Start, data, 4);
   memcpy(&Checksum_End, (data + dataLen - 4), 4);

   if((fileTypeBin != fType) && (fileTypeCyg != fType))
      retVal = FALSE;
   //Received binary file should have a valid checksum
   else if ((fileTypeBin == fType) &&
	((CHECKSUM_APPDCP != Checksum_Start) && (CHECKSUM_COMPLETEDCP != Checksum_Start)
	&&(CHECKSUM_APPDCP != Checksum_End) && (CHECKSUM_COMPLETEDCP != Checksum_End)
	&& (CHECKSUMLCDA != Checksum_Start)))
      retVal = FALSE;
   else
      retVal = upLoadQ.Write(0, msg, 0);

   if (!retVal) delete data;
   return retVal;
}

