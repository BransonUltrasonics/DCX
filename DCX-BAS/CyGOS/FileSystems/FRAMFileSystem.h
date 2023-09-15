/*
 * FRAMFileSytem.h
 *
 *  Created on: Mar 22, 2012
 *      Author: rjamloki
 */

#ifndef FRAMFILESYTEM_H_
#define FRAMFILESYTEM_H_
#include "FileSystem.h"
#include "BlockDevice.h"

enum FRAMFileSystemType
{
	FS_FRAM = 0xC8
};

enum FRAMFileAttribute
{
	faNormal = 0x00, faDirectory = 0x10,
	/**
	 * Special attribute, will let us match all entries regardless of
	 * specifics...
	 */
	faEverything = 0x80,
};

class FRAMFileSystem: public FileSystem
{
	struct FRAMFsFileFind: public FileFindStruct
	{
		char name[100];
		char nameToFind[100];
		/** length: File length in bytes. */
		unsigned int length;
		int fileEntrySectorNumber;
	};
	class FRAMFile: public File
	{
	public:
		/** firstFileSector: First file sector in the TAR file */
		int firstFileSector;
	};
	struct DirEntry
	{
		/** File name */
		char name[100];
		/** File mode */
		char mode[8];
		/** Owner's UID */
		char uid[8];
		/** Owner's GID */
		char gid[8];
		/** File size in bytes */
		char size[12];
		/** Last modification time in Unix format */
		char mtime[12];
		/** Checksum for header block */
		char chksum[8];
		/** Link indicator (file type) */
		char typeflag;
		/** Name of linked file */
		char linkname[100];
		/** UStar indicator */
		char magic[6];
		/** UStar version */
		char version[2];
		/** Owner user name */
		char uname[32];
		/** Owner group name */
		char gname[32];
		/** Device major number */
		char devmajor[8];
		/** Device minor number */
		char devminor[8];
		/** File name prefix */
		char prefix[155];
		/** Padding to make header a sector in size (512 bytes) */
		char padding[12];
	};
public:
	FRAMFileSystem(BlockDevice * device, int first, int length);
	virtual ~FRAMFileSystem();
	static void RegisterFileSystems(FileSystemManager * fsm, int prioMin,
			int prioMax);
protected:
	// Find file functions
	virtual File * MsgFindOpen(const char * fileName, int attrib);
//	virtual int MsgFindFile(FileFind & ff, void * search);
	virtual int MsgFindClose(void * search);
	// File content functions
	virtual int MsgFileOpen(const char * fileName, int mode, File ** file);
	virtual int MsgFileClose(File * file);
	virtual int MsgFileRead(File * file, void * buffer, int * length);
	virtual int MsgFileWrite(File * file, const void * buffer, int * length);
	virtual int MsgFileSeek(void * file, int offset, int mode);
	virtual int MsgFileLength(void * file);
	// File and disk management functions
	virtual int MsgFileRemove(const char * path);
	virtual int MsgFileUnlink(const char * fileName);
	virtual int MsgFileRename(const char * fileDirName, char * newName);
	virtual int MsgCreateDir(const char * DirName);
	virtual int MsgRemoveDir(const char * DirName);
	//
	int OpenFile(const char * fileName, int openMode, File ** file);
	int FindFirst(const char * fileName, FRAMFsFileFind * ffblk, int attrib);
	int GetSectorNumber(int firstSector, int offset)
	{
		return firstSector + (offset / device->GetSectorSize());
	}
	//
	static FileSystem * CreateFileSystem(BlockDevice * device, int type,
			int first, int length);
	//
	unsigned char * sectorBuf;
	DirEntry dir;
};


#endif /* FRAMFILESYTEM_H_ */
