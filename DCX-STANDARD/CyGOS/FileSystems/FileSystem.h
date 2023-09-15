// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/FileSystems/FileSystem.h_v   1.1   05 Aug 2011 10:21:40   pdwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares the base file system task object.

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "Task.h"
#include "FIFO.h"
#include "FileSystemManager.h"

class BlockDevice;

struct FileFind;

/**
 * File system class. File system does not know anything about errno variable,
 * this is part of C runtime library as implemented in io.h/cpp. All functions
 * return integers with values from errno.h.
 */
class FileSystem: public Task
{
	friend class FileSystemManager;
	enum FileSystemMessageType
	{
		fsmFindOpen,
		fsmFindFile,
		fsmFindClose,
		fsmOpen,
		fsmClose,
		fsmRead,
		fsmWrite,
		fsmSeek,
		fsmLength,
		fsmRemove,
		fsmUnlink,
		fsmRename,
		fsmCreateDir,
		fsmRemoveDir,
	};
	struct FileSystemMessage
	{
		/** Message type */
		FileSystemMessageType type;
		/** File or path name, if any */
		const char * fileName;
		/** Target file name for rename */
		char * rnmfileName;
		/** File attributes TODO what attributes? */
		int attrib;
		/** File open mode as in io.h */
		int mode;
		/** File find structure */
		FileFind * ff;
		/** TODO used to be file find support */
		void * search;
		/** File pointer for open file operations */
		File * file;
		/** Location where to return new open file pointer */
		File ** ffile;
		/** Read/write buffer */
		void * buff;
		/** Write length, maximum on entry, axtual on exit */
		int * len;
		/** Reply queue for the caller task, all return codes are integers */
		Fifo<int> * intReply;
	};
public:
	FileSystem(char * name, int stackSize, BlockDevice * dev, int first,
			int length);
	virtual ~FileSystem();
	static void RegisterFileSystems(FileSystemManager * fsm);
	int FileClose(File * file);
	int FileLength(File * file);
	int FileRead(File * file, void * buffer, int * length);
	int FileWrite(File * file, const void * buffer, int * length);
protected:
	void Run();
	int FileOpen(const char * path, int oflags, File ** file);
	int Remove(const char * path);
	int Unlink(const char * path);
	int ReadSector(int sector, unsigned char * buffer);
	int WriteSector(int sector, unsigned char * buffer);
	// Find file functions
	virtual File * MsgFindOpen(const char * fileName, int attrib) = 0;
	//virtual int MsgFindFile(FileFind & ff, void * search) = 0;
	virtual int MsgFindClose(void * search) = 0;
	// File content functions
	virtual int MsgFileOpen(const char * fileName, int mode, File ** file) = 0;
	virtual int MsgFileClose(File * file) = 0;
	virtual int MsgFileRead(File * file, void * buffer, int * length) = 0;
	virtual int MsgFileWrite(File * file, const void * buffer, int * length) = 0;
	virtual int MsgFileSeek(void * file, int offset, int mode) = 0;
	virtual int MsgFileLength(void * file) = 0;
	// File and disk management functions
	virtual int MsgFileUnlink(const char * fileName) = 0;
	virtual int MsgFileRemove(const char * path) = 0;
	virtual int MsgFileRename(const char * fileDirName, char * newName) = 0;
	virtual int MsgCreateDir(const char * DirName) = 0;
	virtual int MsgRemoveDir(const char * DirName) = 0;
	/**
	 * device: This device (task) is used for raw sector I/O.
	 */
	BlockDevice * device;
	Fifo<FileSystemMessage> messageQueue;
	int sectorOffset;
	int sectorCount;
};

#endif
