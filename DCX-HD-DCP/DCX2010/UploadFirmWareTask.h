/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/UploadFirmWareTask.h_v   1.0.2.0.1.0   17 Mar 2016 16:23:46   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/UploadFirmWareTask.h_v  $
 * 
 *    Rev 1.0.2.0.1.0   17 Mar 2016 16:23:46   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.0.2.1   17 Mar 2016 16:07:26   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.0.2.0   19 Sep 2014 07:35:26   amaurya
 * Changes for complete.bin upload through website.
 * 
 *    Rev 1.0   02 Mar 2011 08:14:44   PDwivedi
 * Initial revision.
*/
#ifndef UPLOADFIRMWARETASK_H_
#define UPLOADFIRMWARETASK_H_

#include "Task.h"
#include "FIFO.h"
#include "Globals.h"
#include "FirmwareUpgrade.h"
#include "LOG.h"

#define CHECKSUM_COMPLETEDCP	0xDD1267AA
#define CHECKSUM_APPDCP			0xAA1267BB
enum FileType
{
	fileTypeErr,
	fileTypeBin,
	fileTypeCyg,
};
enum FlashArea
{
   FlashAreaInvalid,
   FlashAreaApp,
   FlashAreaWeb,
   FlashAreaBootloader,
   FlashAreaComplete,
};
struct MsgUploadFirmware
{
	FileType fileType;
	int fileLen;
	char *fileData;
	SINT8 *Action;
};
class UploadFirmware: public Task
{
public:
	UploadFirmware();
	bool ProgramFirmware(FileType fType, char * data, int dataLen, SINT8* Action);
	static UploadFirmware * thisPtr;
protected:
	void Run();
	void ProgramFlash(SINT8 * Data, SINT32 DataLen, FlashArea Flasharea);
	void ProgramCompressedTarRegionFlash(char * data, int dataLen);
	MsgUploadFirmware msg;
	Fifo<MsgUploadFirmware> upLoadQ;
};
void EraseCallback(int offset, int length);
void ProgramCallback(int offset, int length);

#endif /* UPLOADFIRMWARETASK_H_ */
