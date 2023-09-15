// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/stdlib/stdio.h_v   1.0   21 Mar 2012 12:49:48   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares stdio.h functions so that we will not have to use
// libraries provided by the compiler. Their overhead is unknown and may
// interfere with our system design. Implementation is simple and does
// not support floating point at the moment. This file shall only contain
// definitions one would reasonably expect to find in standard stdio.h.
// All files are defined using C naming convention, so if C modules must be
// used for any reason, functions declared here are accessible.

#ifndef STDIO_H_
#define STDIO_H_

#include "ArchDefs.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/* Seek method constants */

#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

// Number of streams which the implementation guarantees can be open
// simultaneously. The value is at least eight.
#define FOPEN_MAX 32

struct _FILE
{
};

typedef struct _FILE FILE;
typedef unsigned long fpos_t;

int fclose(FILE *);
int feof(FILE *);
int fgetc(FILE *);
int _fgetchar(void);
int fgetpos(FILE *, fpos_t *);
int fileno(FILE *);
FILE * fopen(const char *, const char *);
int fprintf(FILE *, const char *, ...);
int fputc(int, FILE *);
int _fputchar(int);
int fputs(const char *, FILE *);
size_t fread(void *, size_t, size_t, FILE *);
int fscanf(FILE *, const char *, ...);
int fsetpos(FILE *, const fpos_t *);
int fseek(FILE *, long, int);
long ftell(FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
int getc(FILE *);
int getchar(void);
char * gets(char *);
int _getw(FILE *);
int printf(const char *, ...);
int putc(int, FILE *);
int putchar(int);
int puts(const char *);
int _putw(int, FILE *);
int remove(const char *);
int rename(const char *, const char *);
void rewind(FILE *);
int sprintf(char *, const char *, ...);
int _unlink(const char *);

int pprintf(const char *, ...);

#if defined(__cplusplus)
}
#endif

#endif
