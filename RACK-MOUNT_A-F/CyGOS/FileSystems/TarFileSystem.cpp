// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/FileSystems/TarFileSystem.cpp_v   1.0   09 Jun 2015 09:13:46   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares the TAR file system task object.

#include "TarFileSystem.h"
#include "BlockDevice.h"
#include "errno.h"
#include "DebugNew.h"

#define printf
#define pprintf

void RegisterTarFS(int prioMin, int prioMax)
{
	TarFileSystem::RegisterFileSystems(fsManager, prioMin, prioMax);
}

TarFileSystem::TarFileSystem(BlockDevice * device, int first, int length) :
	FileSystem("TarFS", 2048*2, device, first, length)
{
	sectorBuf = new unsigned char[device->GetSectorSize()];
}

TarFileSystem::~TarFileSystem()
{
	delete sectorBuf;
}

/**
 * RegisterFileSystems: Register file systems handled by this task with File
 * System Manager.
 * @param fsm File system manager that is responsible for handling file systems
 * handled by this task.
 */
void TarFileSystem::RegisterFileSystems(FileSystemManager * fsm, int prioMin,
		int prioMax)
{
	fsm->RegisterFS(FS_TAR, "TAR", CreateFileSystem, prioMin, prioMax);
}

/**
 * CreateFileSystem: Create a file system task to support file system found on
 * a block device. Function is called by File System Manager when block device
 * media is detected. Function returns file system task object that is
 * dynamically allocated. Task is not started in this function.
 * @param device Block device that is used for storage.
 * @param type File system type byte. Values are registered through
 * RegisterFileSystems function call.
 * @param first First logical sector in the partition, contains drive boot
 * sector and BIOS Parameter Block.
 * @param length Logical drive size in sectors.
 * @return FileSystemTask * Newly created task object to handle the requested
 * file system.
 */
FileSystem * TarFileSystem::CreateFileSystem(BlockDevice * device, int type,
		int first, int length)
{
	ASSERT(type == 0xC6);
	return new TarFileSystem(device, first, length);
}

File * TarFileSystem::MsgFindOpen(const char * fileName, int attrib)
{
	ASSERT(fileName);
	ASSERT(attrib);
	return 0;
}

//int TarFileSystem::MsgFindFile(FileFind & ff, void * search)
//{
//	ASSERT(search);
//	return 0;
//}

int TarFileSystem::MsgFindClose(void * search)
{
	ASSERT(search);
	return 0;
}

int TarFileSystem::MsgFileOpen(const char * fileName, int mode, File ** file)
{
	File * newFile = 0;
	int error = OpenFile(fileName, mode, &newFile);
	if (newFile)
		newFile->fs = this;
	*file = newFile;
	return error;
}

/**
 * OpenFile: Open the file on TAR file system and return the pointer to the
 * file structure. Function sets errno to error values when appropriate,
 * assuming that file system manager cleared the errno right before this
 * function was called.
 * @param fileName File name to open.
 * @param openMode File open mode, read/write, create etc.
 * @param File ** File structure pointer that will be filled out if there are
 * no errors.
 * @return errno value to set, or 0 if there are no errors.
 */
int TarFileSystem::OpenFile(const char * fileName, int openMode, File ** file)
{
	*file = 0;

	// Only read mode supported
	if ((openMode & O_RDONLY) == 0)
		return EROFS;
	// not write but append/truncate/create
	if ((!(openMode & O_WRONLY)) && (openMode & (O_APPEND | O_TRUNC | O_CREAT)))
		return EINVAL;
	TarFsFileFind ff;
	int findError = FindFirst(fileName + 3, &ff, faNormal);
	if (findError != ERROR_SUCCESS)
		return ENOENT;
	TarFile * ffile = new TarFile();
	strcpy(ffile->name, fileName);
	ffile->openMode = O_RDONLY;
	ffile->fileLength = ff.length;
	ffile->fileOffset = 0;
	ffile->firstFileSector = ff.fileEntrySectorNumber + 1;
	*file = ffile;
//	printf("TAR file opened name '%s', size %d, first file sector %d\n",
//			ffile->name, ffile->fileLength, ffile->firstFileSector);
	return ERROR_SUCCESS;
}

// TODO TAR FS: might be useful to provide multi-level directory search...
int TarFileSystem::FindFirst(const char * fileName, TarFsFileFind * ffblk,
		int attrib)
{
	ffblk->fileEntrySectorNumber = 0;
	strcpy(ffblk->nameToFind, fileName);
	for (;;)
	{
	//	pprintf("Sector offset %08X, ", ffblk->fileEntrySectorNumber * 512);
		int error = ReadSector(ffblk->fileEntrySectorNumber,
				(unsigned char *) &dir);
		if (error != ERROR_SUCCESS)
			return error;
	//	pprintf("name '%s'\n", dir.name);
		// We don't need to go further if name is not valid
		if (dir.name[0] == 0)
			return ENOENT;
		if (stricmp(ffblk->nameToFind, dir.name) == 0)
			break;
		int length = otoi(dir.size);
		length = (length + 511) / 512;
		ffblk->fileEntrySectorNumber += length + 1;
	}
	ffblk->length = otoi(dir.size);
	strcpy(ffblk->name, dir.name);
	return ERROR_SUCCESS;
	ASSERT(attrib == faNormal);
}

int TarFileSystem::MsgFileClose(File * file)
{
	delete file;
	return ERROR_SUCCESS;
}

int TarFileSystem::MsgFileRead(File * ffile, void * data, int * length)
{
	TarFile * file = (TarFile *) ffile;
	if (!(file->openMode & O_RDONLY))
		return EINVAL;
	// Quick check for reading past the file. Return ERROR_SUCCESS because EOF
	// condition is not considered an error at this level.
	if (file->fileOffset >= file->fileLength)
	{
		*length = 0;
		return ERROR_SUCCESS;
	}
	// Another test - read before the beginning of file. Again, read size is 0
	// and this is not an error.
	if (file->fileOffset < 0)
	{
		*length = 0;
		return ERROR_SUCCESS;
	}
	// len: This much can be read from the file. It is a lesser value of how
	// much must be read and how much is available.
	int len = *length;
	// Limit the length to what is left from the file...
	if ((file->fileOffset + len) > file->fileLength)
		len = file->fileLength - file->fileOffset;
	// So far we read 0 bytes...
	*length = 0;

	unsigned char * udata = (unsigned char *) data;
	int readSize = len;
	len = 0;
	//	int currFileoffset = file->fileOffset;
	while (readSize > 0)
	{
		// This read access is actually going in chunks not to cross sector
		// boundaries...
		int length = readSize;
		int bytesPerSector = device->GetSectorSize();
		if (length > bytesPerSector)
			length = bytesPerSector;

		int sectorNumber = GetSectorNumber(file->firstFileSector,
				file->fileOffset);
		int error = ReadSector(sectorNumber, sectorBuf);
		if (error != ERROR_SUCCESS)
			return error;
		// Sector boundary check
		int sectorOffset = file->fileOffset & (bytesPerSector - 1);
		if ((length + sectorOffset) > bytesPerSector)
			length = bytesPerSector - sectorOffset;
		// Adjust counters and pointers, transfer the data...
		len += length;
		memcpy(udata, sectorBuf + sectorOffset, length);
		udata += length;
		readSize -= length;
		file->fileOffset += length;
	}
	*length = len;
	return ERROR_SUCCESS;
}

int TarFileSystem::MsgFileWrite(File * file, const void * buffer, int * length)
{
	ASSERT(file);
	ASSERT(buffer);
	ASSERT(length);
	return EROFS;
}

int TarFileSystem::MsgFileSeek(void * f, int offset, int mode)
{
	File * file = (File *) f;
	switch (mode)
	{
	case SEEK_SET:
		file->fileOffset = offset;
		break;
	case SEEK_CUR:
		file->fileOffset += offset;
		break;
	case SEEK_END:
		file->fileOffset = file->fileLength + offset;
		break;
	default:
		return EINVAL;
	}
	return ERROR_SUCCESS;
}

int TarFileSystem::MsgFileLength(void * file)
{
	return ((File *) file)->fileLength;
}

int TarFileSystem::MsgFileUnlink(const char * path)
{
	ASSERT(path);
	return EROFS;
}

int TarFileSystem::MsgFileRemove(const char * path)
{
	ASSERT(path);
	return EROFS;
}

int TarFileSystem::MsgFileRename(const char * name, char * newName)
{
	ASSERT(name);
	ASSERT(newName);
	return EROFS;
}

int TarFileSystem::MsgCreateDir(const char * path)
{
	ASSERT(path);
	return EROFS;
}

int TarFileSystem::MsgRemoveDir(const char * path)
{
	ASSERT(path);
	return EROFS;
}
