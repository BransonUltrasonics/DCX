/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ObjDictionary.cpp_v   1.0   29 Aug 2012 13:31:00   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ObjDictionary.cpp_v  $
 * 
 *    Rev 1.0   29 Aug 2012 13:31:00   rjamloki
 * Initial revision.
*/
#include "ObjDictionary.h"
#include "stdio.h"
#include "assert.h"


/**
 * ObjDictionary: Create an object dictionary that points
 * to one of the standard Modbus register table.
 */
ObjDictionary::ObjDictionary(MODBusODEntry const *  OD)
{
   Dict = OD;
}

/* const MODBusODEntry * ObjDictionary::FindOdEntry(UINT16 Index, UINT16 * RegIndex)
 *
 * 	 Purpose:
 *      This function is used to find the modbus table entry the passed index
 *      parameter belongs to. If table has an entry with index x and count n.
 *		The entry will be returned if the passed Index parameter lies in
 *		in the range x to x - n - 1.
 *		This function is called by MBRegHoldingCB() function.
 *   Entry condition:
 *		Index = The Index corresponding to which modbus table entry is to be found.
 *		RegIndex = Pointer where offset of passed Index parameter from the start index
 *		 present in modbus table is to be filled.
 *
 *   Exit condition:
 *		Returns the pointer to the table entry the passed Index parameter belongs to
 *		else returns NULL.
 */
const MODBusODEntry * ObjDictionary::FindOdEntry(UINT16 Index, UINT16 * RegIndex)
{
   const MODBusODEntry * Temp = NULL;
   const MODBusODEntry * Entry = Dict;
   ASSERT(Index > 0);
   UINT16 TempRegIndex = 0;

   while((Temp == NULL) && (Entry -> Index != 0)){
      if((Index >= Entry -> Index) && (Index < (Entry -> Index + Entry -> Count))){
    	 Temp = Entry;
		 TempRegIndex = Index - Entry -> Index;
      }
	  Entry++;
   }
   * RegIndex = TempRegIndex;
   return Temp;
}


/* const MODBusODEntry * ObjDictionary::FindOdEntry(UINT16 Index)
 *
 * 	 Purpose:
 *      This function is used to find the Modbus table entry corresponding to the
 *      the passed Index.
 *		This function is called by MBRegHoldingCB() function.
 *   Entry condition:
 *		Index: The Index corresponding to which modbus table entry is to be found.
 *
 *   Exit condition:
 *		Returns the pointer to the table entry whose Index matches with the one passed
 *		in Index parameter else returns NULL.
 */
const MODBusODEntry * ObjDictionary::FindOdEntry(UINT16 Index)
{
	const MODBusODEntry * Temp = NULL;
	const MODBusODEntry * Entry = Dict;
	ASSERT(Index > 0);
	while((Temp == NULL) && (Entry -> Index != 0)){
		if(Index == Entry -> Index)
			Temp = Entry;
		 Entry++;
	}
	return Temp;
}
