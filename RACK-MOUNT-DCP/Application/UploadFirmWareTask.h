/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UploadFirmWareTask.h_v   1.3   19 Mar 2015 07:11:04   sesharma  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UploadFirmWareTask.h_v  $
 * 
 *    Rev 1.3   19 Mar 2015 07:11:04   sesharma
 * Branson Coding Standard Applied.
 * 
 *    Rev 1.2   17 Sep 2014 09:04:28   amaurya
 * Added checksum define
 * 
 *    Rev 1.1   03 Sep 2014 07:43:22   amaurya
 * Added Checksum define for LCD.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:30   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:06   amaurya
 * Initial revision.
 */

#ifndef UPLOADFIRMWARETASK_H_
#define UPLOADFIRMWARETASK_H_

#include "Task.h"
#include "FIFO.h"
#include "Globals.h"
#include "FirmwareUpgrade.h"
#include "LOG.h"
#include "BDMTask.h"

#define CHECKSUMLCDA				0x8B899EFE//0xEE1267AA
#define CHECKSUM_COMPLETEDCP	0xDD1267AA
#define CHECKSUM_APPDCP			0xAA1267BB

enum FileType {
	fileTypeErr, fileTypeBin, fileTypeCyg,
};
enum FlashArea {
	FlashAreaInvalid,
	FlashAreaApp,
	FlashAreaWeb,
	FlashAreaBootloader,
	FlashAreaComplete,
};
struct MsgUploadFirmware {
	FileType fileType;
	int fileLen;
	char *fileData;
	SINT8 *Action;
};
class UploadFirmware: public Task {
public:
	UploadFirmware();
	bool ProgramFirmware(FileType fType, char * data, int dataLen,
	      SINT8* Action);
	static UploadFirmware * thisPtr;
protected:
	void Run();
	void ProgramFlash(SINT8 * Data, SINT32 DataLen, FlashArea Flasharea);
	void ProgramCompressedTarRegionFlash(char * data, SINT32 dataLen);
	void SendLCDAdata(SINT8 * Data, SINT32 DataLen);
	MsgUploadFirmware msg;
	Fifo<MsgUploadFirmware> upLoadQ;
};
void EraseCallback(int offset, int length);
void ProgramCallback(int offset, int length);

#endif /* UPLOADFIRMWARETASK_H_ */
