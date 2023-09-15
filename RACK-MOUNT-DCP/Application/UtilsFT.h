/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UtilsFT.h_v   1.2   25 Jul 2016 15:13:40   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UtilsFT.h_v  $
 * 
 *    Rev 1.2   25 Jul 2016 15:13:40   GButron
 * TrimLeading and DoubleParts functions prototypes removed
 * 
 *    Rev 1.1   07 Jun 2016 12:11:56   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   06 Jun 2016 16:33:06   GButron
 * Initial revision.
*/

#ifndef UTILSFT_H_
#define UTILSFT_H_
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "portable.h"

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')

typedef void (*CallBackFuncTokenize)(STR *);

class UtilsFT {
public:
	static void Clean(STR *ValuePtr);
	static BOOLEAN IsNumeric(STR *ValuePtr);
	static BOOLEAN IsEmpty(STR *ValuePtr);
	static void Tokenize(STR *ValuePtr,STR DelimitStr,CallBackFuncTokenize CallBackFunction);
	static DOUBLE Atof(const SINT8 *InputPtr);
	static STR* SubString(const STR* InputPtr,UINT16 Offset,SINT16 Len, STR* DestPtr);
	static SINT16 IndexOf(const STR* ValuePtr,STR SearchStr);
	static void StringFloatParts(const STR* ValuePtr,STR* NumberPtr,STR* DecimalPtr);
};
#endif /* UTILSFT_H_ */
