// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/stdlib/String.cpp_v   1.1   14 Dec 2010 11:40:56   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements string.h functions so that we will not have to use
// libraries provided by the compiler. Their overhead is unknown and
// may interfere with our system design. ASM generated by the compiler is good
// enough so we don't need to go bit-twiddling with these functions.

#include "string.h"
#include "stdlib.h"
#include "DebugNew.h"

#if 0
/**
 * memcpy: Memory copy. Function handles misaligned operands the same way
 * ColdFire does - using extra bus cycles. No attempt is made to provide
 * code that for equally misaligned buffers copies few bytes to align the
 * pointers, then copies longs and finally copies the leftover bytes.
 * @param dst Destination address
 * @param src Source address
 * @param len Data length
 * @return void* By convention, destination pointer is returned.
 */
void * memcpy(void * dst, const void * src, int len)
{
	union Pointer
	{
		int i;
		void * v;
		char * c;
		unsigned long * l;
		unsigned long long * ull;
	};
	Pointer s, d;
	s.v = (void *) src;
	d.v = dst;
	if (((d.i | s.i) & 3) == 0)
	{
		// if both source and destination are longword-aligned, transfer data in
		// longwords and adjust character pointers and count as appropriate.
		// 8-byte transfers are about 15% more efficient than 4-bytes
		int l = len >> 3;
		while (l--)
		{
			*d.ull++ = *s.ull++;
		}
		len &= 7;
		while (len >= 4)
		{
			*d.l++ = *s.l++;
			len -= 4;
		}
	}
	while (len--)
		*d.c++ = *s.c++;
	return dst;
}
#endif

/**
 * memset: Set memory buffer at <b>s</b>, length <b>n</b> bytes to <b>c</b>.
 * @param s Buffer address
 * @param c Byte to set to
 * @param n Length of the buffer
 * @return void* By convention, buffer pointer is returned.
 */
void * memset(void *s, int c, unsigned n)
{
	char * cd = (char *) s;
	int is = (int) s;
	if ((is & 3) == 0)
	{
		unsigned int * ud = (unsigned int *) s;
		unsigned int value = c;
		value |= value << 8;
		value |= value << 16;
		int l = n >> 2;
		while (l--)
			*ud++ = value;
		cd += n & 0xFFFFFFFC;
		n &= 3;
	}
	while (n--)
		*cd++ = c;
	return s;
}

/**
 * strlen: Return length of the string, not counting terminating zero.
 * @param str The string.
 * @return int The length.
 */
int strlen(const char *str)
{
	int len = 0;
	if (str == 0)
		return 0;
	while (*str++ != '\0')
		++len;
	return len;
}

/**
 * strcat: Append second string to the end of the first. Memory overruns are
 * not detected.
 * @param dst Destination string.
 * @param src String to append.
 * @return char* Destination string.
 */
char * strcat(char * dst, const char * src)
{
	char * dst1 = dst;
	while (*dst1)
		dst1++;
	while (*src)
		*dst1++ = *src++;
	*dst1 = 0;
	return dst;
}

/**
 * strcpy: Copy a string.
 * @param dst Destination string.
 * @param src Source string.
 * @return char* Destination string.
 */
char * strcpy(char * dst, const char * src)
{
	char * dst1 = dst;
	while (*src)
		*dst1++ = *src++;
	*dst1 = 0;
	return dst;
}

/**
 * memcmp: Compare one region in memory to another.
 * @param p1 First region.
 * @param p2 Second region.
 * @param len Length of each region.
 * @return int &lt;0 p1 less than p2, 0 p1 identical to p2, &gt;0 p1 greater
 * than p2
 */
int memcmp(const void * p1, const void * p2, int len)
{
	const signed char * cp1 = (const signed char *) p1;
	const signed char * cp2 = (const signed char *) p2;
	int diff = 0;
	while (len--)
	{
		diff = *cp1++ - *cp2++;
		if (diff)
			break;
	}
	return diff;
}

/**
 * strcmp: Compare one string in memory to another.
 * @param s1 First string.
 * @param s2 Second string.
 * @return int &lt;0 s1 less than s2, 0 s1 identical to s2, &gt;0 s1 greater
 * than s2
 */
int strcmp(const char * s1, const char * s2)
{
	int diff = 0;
	for (;;)
	{
		if ((*s1 == 0) && (*s2 == 0))
			return 0;
		diff = *s1++ - *s2++;
		if (diff)
			break;
	}
	return diff;
}

/**
 * memmove: Copies n bytes between two memory areas; The areas may overlap.
 * Routine is not the most efficient because it uses temporary allocated buffer
 * for storage between copies.
 * @param dst Destination buffer.
 * @param src Source buffer.
 * @param n No of bytes to copy.
 */
void * memmove(void * dst, const void * src, unsigned int n)
{
	unsigned char * buf = new unsigned char[n];
	memcpy(buf, src, n);
	memcpy(dst, buf, n);
	delete buf;
	return dst;
}

/**
 * memchr: find character in memory block
 * @param str String to search for
 * @param ch  Character to search
 * @param n   Number of bytes beyond which stop searching
 */
void * memchr(const void *str, int ch, int n)
{
	const unsigned char * s = (const unsigned char *) str;
	unsigned char c = ch;
	if (n > 0)
	{
		n--;
		do
		{
			if (*s++ == c)
			{
				return (void *) (s - 1);
			}
		} while (n--);
	}
	return NULL;
}

/**
 * strdup: Allocate a copy of a string.
 * @param str Null-terminated source string.
 * @return char * A pointer to the storage location for the copied string or
 * NULL if storage cannot be allocated.
 */
char * strdup(const char * str)
{
	char * string = new char[strlen(str) + 1];
	if (string)
		strcpy(string, str);
	return string;
}

/**
 * strchr: Find a character in a string.
 * @param str Null-terminated source string.
 * @param ch Character to be located.
 * @return char * A pointer to the first occurrence of c in string, or NULL if
 * c is not found.
 */
char * strchr(const char *str, int ch)
{
	while (*str)
	{
		if (*str == ch)
			return (char *) str;
		str++;
	}
	return 0;
}

/**
 * stricmp: Compare characters of two strings without regard to case.
 * @param s1 Null-terminated string to compare.
 * @param s2 Null-terminated string to compare.
 * @return int &lt;0 s1 less than s2, 0 identical, &gt;0 s1 greater than s2.
 */
int stricmp(const char * s1, const char * s2)
{
	while (*s1 && *s2)
	{
		int c1 = *s1++;
		if ((c1 >= 'A') && (c1 <= 'Z'))
			c1 |= 0x20;
		int c2 = *s2++;
		if ((c2 >= 'A') && (c2 <= 'Z'))
			c2 |= 0x20;
		if (c1 != c2)
			return c1 - c2;
	}
	return *s1 - *s2;
}

/**
 * strnicmp: Compare characters of two strings without regard to case. Compare
 * is limited to N characters.
 * @param s1 Null-terminated string to compare.
 * @param s2 Null-terminated string to compare.
 * @param n Number of characters to compare.
 * @return int &lt;0 s1 less than s2, 0 identical, &gt;0 s1 greater than s2.
 */
int strnicmp(const char * s1, const char * s2, int n)
{
	while ((*s1) && (n > 0))
	{
		int c1 = *s1++;
		if ((c1 >= 'A') && (c1 <= 'Z'))
			c1 |= 0x20;
		int c2 = *s2++;
		if ((c2 >= 'A') && (c2 <= 'Z'))
			c2 |= 0x20;
		if (c1 != c2)
			return c1 - c2;
		n--;
	}
	return 0;
}

/**
 * strncmp: Compare characters of two strings. Compare is limited to N
 * characters.
 * @param s1 Null-terminated string to compare.
 * @param s2 Null-terminated string to compare.
 * @param n Number of characters to compare.
 * @return int &lt;0 s1 less than s2, 0 identical, &gt;0 s1 greater than s2.
 * /
int strncmp(const char * s1, const char * s2, int n)
{
	while ((*s1) && (n > 0))
	{
		int c1 = *s1++;
		int c2 = *s2++;
		if (c1 != c2)
			return c1 - c2;
		n--;
	}
	return 0;
}*/

int strncmp(const char *s1, const char *s2, int n)
 {
     unsigned char uc1, uc2;
     /* Nothing to compare?  Return zero.  */
     if (n == 0)
         return 0;
     /* Loop, comparing bytes.  */
     while (n-- > 0 && *s1 == *s2) {
         /* If we've run out of bytes or hit a null, return zero
            since we already know *s1 == *s2.  */
         if (n == 0 || *s1 == '\0')
             return 0;
         s1++;
         s2++;
     }
     uc1 = (*(unsigned char *) s1);
     uc2 = (*(unsigned char *) s2);
     return ((uc1 < uc2) ? -1 : (uc1 > uc2));
 }

/**
 * strupr: Convert a string to uppercase.
 * @param str String to capitalize.
 * @return char * Function returns a pointer to the altered string. Because the
 * modification is done in place, the pointer returned is the same as the
 * pointer passed as the input argument.
 */
char * strupr(char *str)
{
	for (char * p = str; *p; p++)
	{
		if ((*p >= 'a') && (*p <= 'z'))
			*p -= ' ';
	}
	return str;
}

/**
 * strrchr: Scan a string for the last occurrence of a character.
 * @param str Null-terminated string to search.
 * @param ch Character to be located.
 * @return char * Function returns a pointer to the last occurrence of ch in
 * string, or NULL if ch is not found.
 */
char * strrchr(const char *str, int ch)
{
	char * ptr = (char *) str;
	ptr += strlen(str);
	while (ptr != str)
	{
		if (ptr[-1] == ch)
			return ptr - 1;
		ptr--;
	}
	return 0;
}

/**
 * strstr: Scan a string for the occurrence of the search string.
 * @param string Null-terminated string to search.
 * @param search Null-terminated string to search for.
 * @return char * A pointer to the first occurrence of search in string, or 0
 * if search does not appear in string. If search points to a string of zero
 * length, the function returns string. Function removes const-ness from the
 * input string.
 */
char * strstr(const char * string, const char * search)
{
	if (!search)
		return (char *) string;
	if (search[0] == 0)
		return (char *) string;
	if (!string)
		return 0;
	char * substring = (char *) string;
	int length = strlen(search);
	while (substring)
	{
		if (strncmp(substring, search, length) == 0)
			break;
		substring = strchr(substring + 1, search[0]);
	}
	return substring;
}
/*
 *strncpy Copy n character of source to n
 *@param dst Destination string.
 *@param src Source string
 *@param n No of characters
 *@return char* Destination string.
 */
char* strncpy(char* dst,const char * src,int n)
{
	int i=0;
	char * dst1 = dst;
	while (*src && i<n)
			{*dst1++ = *src++;
			i++;}
	*dst1 = 0;
	return dst;
}

