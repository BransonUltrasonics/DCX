// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/MultiBoot/SerialBootTask.h_v   1.0   18 Aug 2014 09:16:26   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This module implements the serial port boot loader task.

#ifndef SERIALBOOTTASK_H_
#define SERIALBOOTTASK_H_

#include "Task.h"
#include "Flash.h"
#include "LinkedList.h"

class SerialBootTask: public Task
{
public:
	SerialBootTask();
protected:
	virtual void Run();
	void Init(void * romBase);
	void SerialDownload(bool toFlash);
	bool SerialReceiveHexRecord(unsigned char * buf);
	void BurnImageToFlash();
	void MoveImageToSdram();
	void IdentifyFlash();
	void PrintData();
	void PrintPrompts();
	void EnterSerialNumber();
	void EnterMacAddress(unsigned char * macAddress);
	bool CheckPrompts(int ch);
	static void ExecuteFlash();
	static unsigned short * FlashAddress(unsigned short * ramAddress);
	static unsigned int HexToInt(char * & ptr, int length);
	PtrList<Flash> list;
	Flash * flash;
	static unsigned char * bootImageBuffer;
	static unsigned int bootImageLength;
	static unsigned int bootImageFlash;
};

#endif
