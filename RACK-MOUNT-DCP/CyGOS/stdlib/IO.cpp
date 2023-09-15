// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/stdlib/IO.cpp_v   1.0   18 Aug 2014 09:16:48   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares io.h functions so that we will not have to use
// libraries provided by the compiler. Their overhead is unknown and may
// interfere with our system design. Implementation is simple and does
// not support floating point at the moment. This file shall only contain
// definitions one would reasonably expect to find in standard io.h.
// All files are defined using C naming convention, so if C modules must be
// used for any reason, functions declared here are accessible.

// FIXME I/O API is not fully implemented.
// FIXME If file is open on removable media, media removal should be flagged in the file structure.
 // When file system is lost because of block device media loss, all open files
// on that media must be deallocated but file table entries must be changed to
// some reserved value so that new files cannot be opened with same handle value.
// If file is closed, File pointer needs to be checked against that reserved
// value so that file can be deallocated and handle freed u p without requiring
// file system to be available.

#include "io.h"
#include "FileSystemManager.h"
#include "FileSystem.h"
#include "errno.h"

// All io.h functions clear or set errno to specific error codes. The only
// POSIX-non-compliant part is that errno is reset to 0 in each function if
// there are no errors.

int errno;

static File * fileTable[OPEN_MAX];
/**
 * This is a placeholder for files that are currently open on disconnected
 * block devices. File handle must be reserved so that code accessing some
 * handle from removable device will not access another file that was created
 * later, with the same handle on different block device.
 */
static File * fileDisconnected = (File *) 1;

/**
 * Verify that file handle is valid. Function sets errno to EBADF if handle
 * value is out of range or there is no file object associated with the handle.
 * Also, if file object is deallocated because the block device was
 * disconnected, the error is reported, but only if the caller function must
 * have a valid handle, such as read() or write(). close() does not require the
 * file object to be present, since the object was deallocated when block device
 * disconnect was detected, and the object pointer was replaced with the
 * placeholder. Placeholder is handled separately in close() and error is
 * reported.
 */
static bool VerifyFile(int fd, bool disconnectOK)
{
	errno = EBADF;
	if ((fd < 0) || (fd >= OPEN_MAX))
		return false;
	File * file = fileTable[fd];
	if (!file)
		return false;
	errno = EINTR;
	if ((!disconnectOK) && (file == fileDisconnected))
		return false;
	return true;
}

long _findfirst(const char *, struct _finddata_t *);
int _findnext(long, struct _finddata_t *);
int _findclose(long);

int remove(const char * path)
{
	errno = fsManager->Remove(path);
	return errno == ERROR_SUCCESS ? 0 : -1;
}

int rename(const char *, const char *);
int access(const char *, int);
int chmod(const char *, int);

int close(int fd)
{
	// Close - accept closing disconnected files
	if (!VerifyFile(fd, true))
		return -1;
	File * file = fileTable[fd];
	// If disconnected, just release the handle and pretend nothing happened
	if (file == fileDisconnected)
	{
		errno = EINTR;
		fileTable[fd] = 0;
		return -1;
	}
	errno = file->fs->FileClose(file);
	fileTable[fd] = 0;
	return errno ? -1 : 0;
}

int creat(const char * path, int mode)
{
	return open(path, O_WRONLY | O_CREAT | O_TRUNC);
	mode = 0;
}

int eof(int);
long filelength(int);
long lseek(int, long, int);

int open(const char * path, int oflag, ...)
{
	File * file;
	errno = fsManager->FileOpen(path, oflag, &file);
	if (errno != ERROR_SUCCESS)
		return -1;
	CriticalSection s;
	for (int i = 0; i < OPEN_MAX; i++)
	{
		if (!fileTable[i])
		{
			fileTable[i] = file;
			return i;
		}
	}
	// TODO close the file by pointer in case when table is full after file was opened
	errno = EMFILE;
	delete file;
	return -1;
}

int read(int fd, void * buf, unsigned int nbyte)
{
	if (!VerifyFile(fd, false))
		return -1;
	File * file = fileTable[fd];
	int result = nbyte;
	errno = file->fs->FileRead(file, buf, &result);
	return errno ? -1 : result;
}

long tell(int);

int unlink(const char * path)
{
	errno = fsManager->Unlink(path);
	return errno ? -1 : 0;
}

int write(int fd, const void * buf, unsigned int nbyte)
{
	if (!VerifyFile(fd, false))
		return -1;
	File * file = fileTable[fd];
	int result = nbyte;
	errno = file->fs->FileWrite(file, buf, &result);
	return errno ? -1 : result;
}
