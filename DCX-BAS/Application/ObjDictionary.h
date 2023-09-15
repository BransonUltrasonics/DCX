/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ObjDictionary.h_v   1.2   24 Aug 2013 17:14:06   amaurya  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2013        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ObjDictionary.h_v  $
 * 
 *    Rev 1.2   24 Aug 2013 17:14:06   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.1   10 Sep 2012 03:12:26   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.0   29 Aug 2012 13:31:00   rjamloki
 * Initial revision.
*/
#ifndef OBJDICTIONARY_H_
#define OBJDICTIONARY_H_

#include "portable.h"
/* function call back*/
typedef void (* funCallback)();

struct MODBusODEntry
{
	short Index;//modbus registers start index corresponding to the modbus table entry.
	short Count;//number of holding registers present at the index.
				//If the count is n and start Index(in member variable Index above) is X.
				//The indexes for registers are X to X + n - 1.
				//Each register have 2 byte size.
	void * Variable;//the variable address the Index belongs to
	funCallback Precallback;//function called before accessing the registers corresponding
							//to a modbus table entry.
	funCallback Postcallback;//function called after accessing the registers corresponding
							//to a modbus table entry.
	int VarSize;
};

class ObjDictionary  
{
	public:
		ObjDictionary(MODBusODEntry const *  OD);
		MODBusODEntry const *  Dict;
		const MODBusODEntry * FindOdEntry(UINT16 Index, UINT16 * RegIndex);
		const MODBusODEntry * FindOdEntry(UINT16 Index);
};

#endif
