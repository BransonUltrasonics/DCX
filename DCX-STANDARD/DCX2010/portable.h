/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/portable.h_v   1.1   29 Nov 2010 07:47:38   PDwivedi  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2009        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/portable.h_v  $
 * 
 *    Rev 1.1   29 Nov 2010 07:47:38   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:44   ygupta
 * Initial revision.

******************************************************************************/

#ifndef PORTABLE_H_
#define PORTABLE_H_

typedef  unsigned char       UINT8;           /* 8-bit  unsigned quantity  */
typedef  UINT8               BYTE;            /* 8-bit  unsigned quantity  */
typedef  unsigned short int  UINT16;          /* 16-bit unsigned quantity  */
typedef  unsigned long int   UINT32;          /* 32-bit unsigned quantity  */
typedef  char                SINT8;           /* 8-bit  signed quantity    */
typedef  signed short int    SINT16;          /* 16-bit signed quantity    */
typedef  signed long int     SINT32;          /* 32-bit signed quantity    */
typedef  signed long int     SIZE_T;          /* signed size difference    */
typedef  unsigned long int   PTRDIFF_T;       /* unsigned size difference  */
typedef  signed int          FPOS_T;          /* signed size difference    */
typedef  float               FLOAT;           /* small float               */
typedef  float               DFLOAT;          /* medium float              */

/* Defined to standardize string definitions for the menu structures  */
/* incorporating foreign languages                                    */
typedef  char                STR;
typedef  bool BOOLEAN;
//{
//   FALSE = 0,
//   TRUE	 = 1
//   } BOOLEAN;                                /* system BOOLEANean definition */

/*----------------------------*/
/* Bit manipulation patterns  */
/*----------------------------*/

/* BYTEWISE, WORDWISE or LONGWISE 	*/
#define BIT(x) (1 << x)

#endif /* PORTABLE_H_ */
