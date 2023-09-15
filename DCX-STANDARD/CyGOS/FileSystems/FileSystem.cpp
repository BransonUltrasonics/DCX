// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/FileSystems/FileSystem.cpp_v   1.1   05 Aug 2011 10:21:28   pdwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares the base file system task object.

// FIXME File find API needs review.

#include "FileSystem.h"
#include "BlockDevice.h"
#include "RTOS.h"
#include "assert.h"
#include "stdio.h"

FileSystem::FileSystem(char * name, int stackSize, BlockDevice * dev,
		int first, int length) :
	Task(name, stackSize), messageQueue(10, "FSQueue")
{
	device = dev;
	sectorOffset = first;
	sectorCount = length;
}

FileSystem::~FileSystem()
{
}

void FileSystem::RegisterFileSystems(FileSystemManager * fsm)
{
	if(!fsm)//to disbale compiler warning unused variable.
	CRASH("Base class RegisterFileSystems() called");
	//Avoid unused variable warning.
}

void FileSystem::Run()
{
	for (;;)
	{
		FileSystemMessage msg;
		messageQueue.Read(this, msg, -1);
		int iResult;
		switch (msg.type)
		{
		//		case fsmFindOpen:
		//			vResult = MsgFindOpen(message.fileName, message.mode);
		//			message.fileReply->Write(this, vResult, -1);
		//			break;
		//		case fsmFindFile:
		//			iResult = MsgFindFile(*message.ff, message.search);
		//			message.intReply->Write(this, iResult, -1);
		//			break;
		//		case fsmFindClose:
		//			iResult = MsgFindClose(message.search);
		//			message.intReply->Write(this, iResult, -1);
		//			break;
		case fsmOpen:
			iResult = MsgFileOpen(msg.fileName, msg.mode, msg.ffile);
			msg.intReply->Write(this, iResult, -1);
			break;
		case fsmClose:
			iResult = MsgFileClose(msg.file);
			msg.intReply->Write(this, iResult, -1);
			break;
		case fsmRead:
			iResult = MsgFileRead(msg.file, msg.buff, msg.len);
			msg.intReply->Write(this, iResult, -1);
			break;
		case fsmWrite:
			iResult = MsgFileWrite(msg.file, msg.buff, msg.len);
			msg.intReply->Write(this, iResult, -1);
			break;
		case fsmSeek:
			iResult = MsgFileSeek(msg.file, *msg.len, msg.mode);
			msg.intReply->Write(this, iResult, -1);
			break;
		case fsmLength:
			iResult = MsgFileLength(msg.file);
			msg.intReply->Write(this, iResult, -1);
			break;
		case fsmUnlink:
			iResult = MsgFileUnlink(msg.fileName);
			msg.intReply->Write(this, iResult, -1);
			break;
		case fsmRemove:
			iResult = MsgFileRemove(msg.fileName);
			msg.intReply->Write(this, iResult, -1);
			break;
		case fsmRename:
			iResult = MsgFileRename(msg.fileName, msg.rnmfileName);
			msg.intReply->Write(this, iResult, -1);
			break;
		case fsmCreateDir:
			iResult = MsgCreateDir(msg.fileName);
			msg.intReply->Write(this, iResult, -1);
			break;
		case fsmRemoveDir:
			iResult = MsgRemoveDir(msg.fileName);
			msg.intReply->Write(this, iResult, -1);
			break;
		default:
			printf("Bad File System Message Type %d\n", msg.type);
		}
	}
}

int FileSystem::ReadSector(int sector, unsigned char * buffer)
{
	return device->ReadSector(sector + sectorOffset, buffer);
}

int FileSystem::WriteSector(int sector, unsigned char * buffer)
{
	return device->WriteSector(sector + sectorOffset, buffer);
}

int FileSystem::FileOpen(const char * path, int oflags, File ** file)
{
	FileSystemMessage message;
	memset(&message, 0 ,sizeof(message));
	message.type = fsmOpen;
	message.fileName = path;
	message.mode = oflags;
	message.ffile = file;
	Fifo<int> reply(1, "OpenFile");
	message.intReply = &reply;
	messageQueue.Write(RTOS::GetCurrentTask(), message, -1);
	int returnValue = 0;
	reply.Read(RTOS::GetCurrentTask(), returnValue, -1);
	return returnValue;
}

/**
 * Close the file that was open on this file system.
 * @param file File to be closed. The file object is deleted by the file system
 * task.
 */
int FileSystem::FileClose(File * file)
{
	FileSystemMessage message;
	memset(&message, 0 ,sizeof(message));
	message.type = fsmClose;
	message.file = file;
	Fifo<int> reply(1, "FileClose");
	message.intReply = &reply;
	messageQueue.Write(RTOS::GetCurrentTask(), message, -1);
	int closed = 0;
	reply.Read(RTOS::GetCurrentTask(), closed, -1);
	return closed;
}

int FileSystem::FileRead(File * file, void * buffer, int * length)
{
	FileSystemMessage message;
	memset(&message, 0 ,sizeof(message));
	message.type = fsmRead;
	message.file = file;
	message.buff = buffer;
	message.len = length;
	Fifo<int> reply(1, "FileRead");
	message.intReply = &reply;
	messageQueue.Write(RTOS::GetCurrentTask(), message, -1);
	int returnValue = -1;
	reply.Read(RTOS::GetCurrentTask(), returnValue, -1);
	return returnValue;
}

int FileSystem::FileWrite(File * file, const void * buffer, int * length)
{
	FileSystemMessage message;
	memset(&message, 0 ,sizeof(message));
	message.type = fsmWrite;
	message.file = file;
	message.buff = (void *) buffer;
	message.len = length;
	Fifo<int> reply(1, "FileRead");
	message.intReply = &reply;
	messageQueue.Write(RTOS::GetCurrentTask(), message, -1);
	int returnValue = -1;
	reply.Read(RTOS::GetCurrentTask(), returnValue, -1);
	return returnValue;
}

int FileSystem::Remove(const char * path)
{
	FileSystemMessage message;
	memset(&message, 0 ,sizeof(message));
	message.type = fsmRemove;
	message.fileName = path;
	Fifo<int> reply(1, "Remove");
	message.intReply = &reply;
	messageQueue.Write(RTOS::GetCurrentTask(), message, -1);
	int returnValue = 0;
	reply.Read(RTOS::GetCurrentTask(), returnValue, -1);
	return returnValue;
}

int FileSystem::Unlink(const char * path)
{
	FileSystemMessage message;
	memset(&message, 0 ,sizeof(message));
	message.type = fsmUnlink;
	message.fileName = path;
	Fifo<int> reply(1, "Unlink");
	message.intReply = &reply;
	messageQueue.Write(RTOS::GetCurrentTask(), message, -1);
	int returnValue = 0;
	reply.Read(RTOS::GetCurrentTask(), returnValue, -1);
	return returnValue;
}

int FileSystem::FileLength(File * file)
{
	FileSystemMessage message;
	memset(&message, 0 ,sizeof(message));
	message.type = fsmLength;
	message.file = file;
	Fifo<int> reply(1, "FileLen");
	message.intReply = &reply;
	messageQueue.Write(RTOS::GetCurrentTask(), message, -1);
	int returnValue = -1;
	reply.Read(RTOS::GetCurrentTask(), returnValue, -1);
	return returnValue;
}

