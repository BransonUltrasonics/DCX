/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/portable.h_v   1.3   06 Jun 2016 15:11:16   GButron  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/portable.h_v  $
 * 
 *    Rev 1.3   06 Jun 2016 15:11:16   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.2   19 Mar 2015 06:56:26   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.1   28 Jan 2015 05:33:26   sesharma
 * Corrections done in Data Types typedefs
 * 
 *    Rev 1.0   18 Aug 2014 09:17:26   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:02   amaurya
 * Initial revision.
 */

#ifndef PORTABLE_H_
#define PORTABLE_H_

typedef unsigned char UINT8; /* 8-bit  unsigned quantity  */
typedef UINT8 BYTE; /* 8-bit  unsigned quantity  */
typedef unsigned short int UINT16; /* 16-bit unsigned quantity  */
typedef unsigned int UINT32; /* 32-bit unsigned quantity  */
typedef char SINT8; /* 8-bit  signed quantity    */
typedef signed short int SINT16; /* 16-bit signed quantity    */
typedef signed int SINT32; /* 32-bit signed quantity    */
typedef unsigned int SIZE_T; /* unsigned size difference    */
typedef signed int PTRDIFF_T; /* signed size difference  */
typedef signed int FPOS_T; /* signed size difference    */
typedef float FLOAT; /* small float               */
typedef float DFLOAT; /* medium float              */
typedef double DOUBLE;

/* Defined to standardize string definitions for the menu structures  */
/* incorporating foreign languages                                    */
typedef char STR;
typedef bool BOOLEAN;
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
