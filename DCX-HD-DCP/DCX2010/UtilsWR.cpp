/* $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/UtilsWR.cpp_v   1.0   Feb 09 2017 16:11:02   hasanchez  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/UtilsWR.cpp_v  $
 * 
 *    Rev 1.0   Feb 09 2017 16:11:02   hasanchez
 * Initial revision.
 * 
 *    Rev 1.1   22 Sep 2016 11:51:54   EGUAJARDO
 * Applied coding standard.
 * 
 *    Rev 1.0   15 Sep 2016 17:28:30   EGUAJARDO
 * Initial revision.
 * 
*/

#include "UtilsWR.h"


/*void UtilsWR::Clean(STR *Value)
 *
 *  Purpose:
 *   	This is function clear the string and replace the array with end of line char.
 *
 *  Entry condition:
 *		Value -
 *
 *  Exit condition:
 *  	None.
 */
void UtilsWR::Clean(STR *ValuePtr) {
    UINT32 CountCtr = 0;
    while(ValuePtr[CountCtr] != '\0') {
    	ValuePtr[CountCtr] = '\0';
    	CountCtr++;
    }
}
