/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/UploadFirmWareTask.h_v   1.0   02 Mar 2011 08:14:44   PDwivedi  $
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
 *    Rev 1.0   02 Mar 2011 08:14:44   PDwivedi
 * Initial revision.
*/
#ifndef UPLOADFIRMWARETASK_H_
#define UPLOADFIRMWARETASK_H_

#include "Task.h"
#include "FIFO.h"

enum FileType
{
	fileTypeErr,
	fileTypeBin,
	fileTypeCyg,
};
struct MsgUploadFirmware
{
	FileType fileType;
	int fileLen;
	char * fileData;
};
class UploadFirmware: public Task
{
public:
	UploadFirmware();
	bool ProgramFirmware(FileType fType, char * data, int dataLen);
	static UploadFirmware * thisPtr;
protected:
	void Run();
	void ProgramFlash(char * data, int dataLen);
	void ProgramCompressedTarRegionFlash(char * data, int dataLen);
	MsgUploadFirmware msg;
	Fifo<MsgUploadFirmware> upLoadQ;
};
void EraseCallback(int offset, int length);
void ProgramCallback(int offset, int length);

#endif /* UPLOADFIRMWARETASK_H_ */
