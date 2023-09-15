// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/BlockDevices/FileSystemManager.h_v   1.0   18 Aug 2014 09:16:14   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares file system manager object.

#ifndef FILESYSTEMMANAGER_H_
#define FILESYSTEMMANAGER_H_

#include "Task.h"
#include "FIFO.h"
#include "LinkedList.h"
#include "CyGOS.h"
#include "io.h"

class FileSystem;
class BlockDevice;

typedef FileSystem * (*CreateFS)(BlockDevice * device, int type, int first,
		int length);

static inline unsigned int GetDWord(const unsigned char data[4])
{
	unsigned int result = data[3];
	result <<= 8;
	result |= data[2];
	result <<= 8;
	result |= data[1];
	result <<= 8;
	result |= data[0];
	return result;
}

static inline unsigned int GetWord(const unsigned char data[2])
{
	unsigned int result = data[1];
	result <<= 8;
	result |= data[0];
	return result;
}

static inline void PutDWord(unsigned char * data, unsigned int value)
{
	data[0] = value;
	value >>= 8;
	data[1] = value;
	value >>= 8;
	data[2] = value;
	value >>= 8;
	data[3] = value;
}

static inline void PutWord(unsigned char * data, unsigned int value)
{
	data[0] = value;
	value >>= 8;
	data[1] = value;
}

struct PartitionRecord
{
	unsigned char status;
	unsigned char chsFirst[3];
	unsigned char type;
	unsigned char chsLast[3];
	unsigned char lba[4];
	unsigned char length[4];
	unsigned int GetStartLBA() const
	{
		return GetDWord(lba);
	}
	unsigned int GetLength() const
	{
		return GetDWord(length);
	}
};

struct MBR
{
	unsigned char code[440];
	unsigned char signature[4];
	unsigned char nulls[2];
	PartitionRecord partition[4];
	unsigned char hex55AA[2];
};

struct FileFindStruct
{
	FileSystem * task;
	int ffAttributes;
};

class File
{
public:
	virtual ~File()
	{
	}
	/** name: File name, 256 characters or less (PATH_MAX). */
	char name[PATH_MAX];
	/** openMode: constants from io.h */
	int openMode;
	/** fileOffset: File access offset. TODO 32-bit only */
	int fileOffset;
	/** fileLength: File length in bytes. TODO 32-bit only */
	int fileLength;
	/**
	 * fs: File system task that handles this file structure (or in most cases
	 * a structure derived from this one.
	 */
	FileSystem * fs;
};

class FileSystemManager: public Task
{
	enum FSCommand
	{
		fsDriveDetected, fsDriveLost,
	};
	struct FSMessage
	{
		FSCommand command;
		BlockDevice * device;
		//		const char * name;
		//		bool write;
		//		Fifo<void *> * result;
	};
	struct FileSystemEntry
	{
		int type;
		const char * name;
		CreateFS create;
		int prioLow;
		int prioHigh;
	};
	struct DriveEntry
	{
		BlockDevice * device;
		const char * letters;
	};
public:
	FileSystemManager();
	virtual ~FileSystemManager();
	void RegisterDrive(BlockDevice * device, const char * letters);
	void RegisterFS(int type, const char * name, CreateFS createFSTask,
			int prioLow, int prioHigh);
	void NotifyMediaDetected(BlockDevice * device);
	void NotifyMediaLost(BlockDevice * device);
	int ReadSector(int drive, int sector, int count, void * buffer);
	int WriteSector(int drive, int sector, int count, void * buffer);
	int InitDriveImage(int drive, void * image, int length);
	int RemoveDrive(int drive);
	int FileOpen(const char * path, int oflag, File ** file);
	int FileRead(void * file, void * buffer, int * length);
	int Remove(const char * path);
	int Unlink(const char * path);		
	int FileLength(void * file);
	void FileClose(void * file);
protected:
	void Run();
	int FindDriveNumber(const char * path);
	void MediaDetected(FSMessage & message);
	void MediaLost(FSMessage & message);
	BlockDevice * FindDrive(int drive);
	Fifo<FSMessage> queue;
	/**
	 * logicalDrive: Table of all logical drives, if present. Letters go from
	 * A to Z.
	 */
	FileSystem * logicalDrive['Z' - 'A' + 1];
	/**
	 * physicalDevices: Table of all block devices, no more than we can use
	 * letters for them.
	 */
	BlockDevice * physicalDevices['Z' - 'A' + 1];
	PtrList<FileSystemEntry> fileSystems;
	PtrList<DriveEntry> drives;
};

extern FileSystemManager * fsManager;

#endif
