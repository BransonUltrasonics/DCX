// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/stdlib/io.h_v   1.0   09 Jun 2015 09:14:02   tayars  $
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

#ifndef IO_H_
#define IO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define O_RDONLY	0x0001
#define O_WRONLY	0x0002
#define O_RDWR		0x0003
#define O_CREAT		0x0010
#define O_TRUNC		0x0020
#define O_APPEND	0x0040

/* File attribute constants for _findfirst() */
#define _A_NORMAL       0x00    /* Normal file - No read/write restrictions */
#define _A_RDONLY       0x01    /* Read only file */
#define _A_HIDDEN       0x02    /* Hidden file */
#define _A_SYSTEM       0x04    /* System file */
#define _A_SUBDIR       0x10    /* Subdirectory */
#define _A_ARCH         0x20    /* Archive file */

#define OPEN_MAX 32
#define PATH_MAX 256

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

typedef long time_t; /* time value */
typedef unsigned long _fsize_t; /* Could be 64 bits for Win32 */

struct _finddata_t
{
	unsigned attrib;
	time_t time_create; /* -1 for FAT file systems */
	time_t time_access; /* -1 for FAT file systems */
	time_t time_write;
	_fsize_t size;
	char name[260];
};

long _findfirst(const char *, struct _finddata_t *);
int _findnext(long, struct _finddata_t *);
int _findclose(long);

int remove(const char * path);
int rename(const char *, const char *);
int access(const char *, int);
int chmod(const char *, int);
int close(int fd);
int creat(const char * path, int mode);
int eof(int);
long filelength(int);
long lseek(int, long, int);
int open(const char * path, int oflag, ...);
int read(int fd, void * buf, unsigned int nbyte);
long tell(int);
int unlink(const char * path);
int write(int fd, const void * buf, unsigned int nbyte);

#ifdef __cplusplus
}
#endif

#endif
