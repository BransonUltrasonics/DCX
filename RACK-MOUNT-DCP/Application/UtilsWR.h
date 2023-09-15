/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UtilsWR.h_v   1.1   22 Sep 2016 11:54:44   EGUAJARDO  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UtilsWR.h_v  $
 * 
 *    Rev 1.1   22 Sep 2016 11:54:44   EGUAJARDO
 * Applied coding standard.
 * 
 *    Rev 1.0   15 Sep 2016 17:30:20   EGUAJARDO
 * Initial revision.
 * 
*/

#ifndef UTILSWR_H_
#define UTILSWR_H_

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "portable.h"


class UtilsWR {
public:
	static void Clean(STR *ValuePtr);
};

#endif /* UTILSWR_H_ */
