/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UtilsFT.cpp_v   1.2   25 Jul 2016 15:05:12   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/UtilsFT.cpp_v  $
 * 
 *    Rev 1.2   25 Jul 2016 15:05:12   GButron
 * Coding standard applied after code review
 * 
 *    Rev 1.1   07 Jun 2016 12:10:10   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   06 Jun 2016 16:25:58   GButron
 * Initial revision.
*/

#include "UtilsFT.h"

#define NEGATIVESIGN                                     -1
#define REMAINSTRSIZE                                     3
#define SUBSTRINGOFFSET                                   1
#define INPUTPTRINCREMENT                                 1
#define VALUEMULTIPLIER                                   10
#define FRACVALUE                                         1
#define EXPONENTMAXIMUMVALUE                              308
#define EXPONENTSCALINGFACTORZERO                         0
#define EXPONENTSCALINGFACTOREIGHT                        8
#define EXPONENTSCALINGFACTORFIFTY                        50
#define SCALE1E50                                         1E50
#define SCALE1E8                                          1E8


/*  void UtilsFT::Clean(STR *ValuePtr)
 *
 *  Purpose:
 *   	This is function clear the string and replace the array with end of line char.
 *
 *  Entry condition:
 *		ValuePtr -
 *
 *  Exit condition:
 *  	None.
 */
void UtilsFT::Clean(STR *ValuePtr)
{
   UINT32 CountCtr = 0;

   while(ValuePtr[CountCtr] != '\0') {
      ValuePtr[CountCtr] = '\0';
    	CountCtr++;
   }
}


/*  BOOLEAN UtilsFT::IsEmpty(STR *ValuePtr)
 *
 *  Purpose:
 *   	This is function check if a string value is empty.
 *
 *  Entry condition:
 *		ValuePtr - String pointer to be check.
 *
 *  Exit condition:
 *  	return TRUE if empty or FALSE if not.
 */
BOOLEAN UtilsFT::IsEmpty(STR *ValuePtr)
{
	BOOLEAN IsEmptyFlag = FALSE;

	if(ValuePtr[0]=='\0' || strlen(ValuePtr)==0)
   	IsEmptyFlag = TRUE;
   return IsEmptyFlag;
}


/*  BOOLEAN UtilsFT::IsNumeric(STR *ValuePtr)
 *
 *  Purpose:
 *   	This function check if the string is a numeric value.
 *
 *  Entry condition:
 *		ValuePtr - String pointer to be validate.
 *
 *  Exit condition:
 *  	Return TRUE if the string has numeric values FALSE if not.
 */
BOOLEAN UtilsFT::IsNumeric(STR *ValuePtr)
{
	UINT32 CountCtr = 0;
	BOOLEAN IsNumericFlag = TRUE;

   while(ValuePtr[CountCtr]!='\0') {
      if(isdigit(ValuePtr[CountCtr])==0) {
    	   IsNumericFlag = FALSE;
      }
      CountCtr++;
   }
   return IsNumericFlag;
}


/*  void UtilsFT::Tokenize(STR *ValuePtr,STR DelimitStr,CallBackFuncTokenize CallBackFunction)
 *
 *  Purpose:
 *   	 This function split a string into parts
 *  Entry condition:
 *		 ValuePtr - String pointer to be split.
 *		 DelimitStr - The string letter to be split
 *		 CallBackFuncTokenize - Callback function to send the results
 *
 *  Exit condition:
 *  	 None.
 */
void UtilsFT::Tokenize(STR *ValuePtr,STR DelimitStr,CallBackFuncTokenize CallBackFunction)
{
	STR *OldsPtr = ValuePtr;
	STR OldDelimitStr = DelimitStr;

	while(OldDelimitStr && *ValuePtr) {
		while(*ValuePtr && (DelimitStr != *ValuePtr))
			ValuePtr++;
		*ValuePtr ^= OldDelimitStr = *ValuePtr; // olddelim = *s; *s = 0;
		CallBackFunction(OldsPtr);
		*ValuePtr++ ^= OldDelimitStr; // *s = olddelim; s++;
		OldsPtr = ValuePtr;
	}
}


/*  DOUBLE UtilsFT::Atof(const SINT8 *InputPtr)
 *
 *  Purpose:
 *   	 This function convert string to double
 *  Entry condition:
 *		 InputPtr - String pointer to be convert.
 *
 *  Exit condition:
 *  	 Signed and scaled floating point result
 */
DOUBLE UtilsFT::Atof(const SINT8 *InputPtr)
{
	UINT16 Frac = 0;
	SINT16 Expon = 0;
	DOUBLE Value = 0;
	DOUBLE Scale = 1.0;
	DOUBLE Sign = 1.0;
	DOUBLE Pow10 = 10.0;
  	const UINT16 Step = 1;

   // Skip leading white space, if any.
   while (white_space(*InputPtr))
   {
      InputPtr += Step;
   }

   // Get sign, if any.
   if (*InputPtr == '-') {
      Sign = NEGATIVESIGN;
      InputPtr += INPUTPTRINCREMENT;
   }
   else if (*InputPtr == '+') {
      InputPtr += Step;
   }

   // Get digits before decimal point or exponent, if any.
   for (Value = 0.0; valid_digit(*InputPtr); InputPtr += INPUTPTRINCREMENT) {
      Value = Value * VALUEMULTIPLIER + (*InputPtr - '0');
   }

   // Get digits after decimal point, if any.
   if (*InputPtr == '.') {
   	InputPtr += Step;
   	while (valid_digit(*InputPtr))
      {
         Value += (*InputPtr - '0') / Pow10;
         Pow10 *= VALUEMULTIPLIER;
         InputPtr += Step;
      }
   }

   // Handle exponent, if any.
   if ((*InputPtr == 'e') || (*InputPtr == 'E')) {
      // Get sign of exponent, if any.
   	InputPtr += Step;
      if (*InputPtr == '-') {
         Frac = FRACVALUE;
         InputPtr += INPUTPTRINCREMENT;
      }
      else if (*InputPtr == '+') {
      	InputPtr += Step;
      }

      // Get digits of exponent, if any.
      for (Expon = 0; valid_digit(*InputPtr); InputPtr += Step) {
         Expon = Expon * VALUEMULTIPLIER + (*InputPtr - '0');
      }

      if (Expon > EXPONENTMAXIMUMVALUE)
         Expon = EXPONENTMAXIMUMVALUE;

      // Calculate scaling factor.
      while (Expon >= EXPONENTSCALINGFACTORFIFTY)
      {
      	Scale *= SCALE1E50;
      	Expon -= EXPONENTSCALINGFACTORFIFTY;
      }

      while(Expon >= EXPONENTSCALINGFACTOREIGHT)
      {
      	Scale *= SCALE1E8;
      	Expon -=  EXPONENTSCALINGFACTOREIGHT;
      }

      while(Expon > EXPONENTSCALINGFACTORZERO)
      {
      	Scale *= VALUEMULTIPLIER;
      	Expon -=  FRACVALUE;
      }
   }
    // Return signed and scaled floating point result.
    return Sign * ((Frac == FRACVALUE) ? (Value / Scale) : (Value * Scale));
}


/*  STR* UtilsFT::SubString(const STR* InputPtr,UINT16 Offset,SINT16 Len, STR* DestPtr)
 *
 *  Purpose:
 *   	 This function get the substring value
 *  Entry condition:
 *		 Value - Double to be split.
 *		 Whole - Integer value
 *		 Decimal - Decimal value
 *
 *  Exit condition:
 *  	None.
 */
STR* UtilsFT::SubString(const STR* InputPtr,UINT16 Offset,SINT16 Len, STR* DestPtr)
{
	STR* CommonDestPtr=0;
	UINT16 Input_Len=strlen(InputPtr);

   if (Offset + Len > Input_Len)
   {
   	CommonDestPtr=NULL;
   }
   else{
      strncpy (DestPtr,InputPtr+Offset,Len);
      CommonDestPtr = DestPtr;
   }
   return CommonDestPtr;
}


/*  SINT16 UtilsFT::IndexOf(const STR* ValuePtr,STR SearchStr)
 *
 *  Purpose:
 *   	 This function find a string and gets it position
 *  Entry condition:
 *     Value - String ValuePtr
 *		 SearchStr - String to find
 *
 *  Exit condition:
 *  	 None.
 */
SINT16 UtilsFT::IndexOf(const STR* ValuePtr,STR SearchStr)
{
   const STR* MovedPtr = strchr(ValuePtr,SearchStr);
   SINT16 MovedCommonPtr = 0;

   if(MovedPtr!=0) {
      MovedCommonPtr = MovedPtr-ValuePtr;
   }
   else{
   	MovedCommonPtr = NEGATIVESIGN;
   }
   return MovedCommonPtr;
}


/*  void UtilsFT::StringFloatParts(const STR* ValuePtr,STR* NumberPtr,STR* DecimalPtr)
 *
 *  Purpose:
 *   	 This function split the integer and the decimal parts
 *  Entry condition:
 *		 ValuePtr - String value
 *		 NumberPtr - Integer part
 *		 DecimalPtr - Decimal part
 *
 *  Exit condition:
 *  	 None.
 */
void UtilsFT::StringFloatParts(const STR* ValuePtr,STR* NumberPtr,STR* DecimalPtr)
{
	STR FloatStr[10] = {0};
	SINT16 PosCrt = 0;
	UINT16 LenCrt = 0;
   STR NumberStr[10] = {0};
   STR DecimalStr[10] = {0};

   strcpy(FloatStr,ValuePtr);
	PosCrt=IndexOf(FloatStr,'.');
	SubString(FloatStr,0,PosCrt,NumberStr);
	LenCrt=strlen(FloatStr);
	SubString(FloatStr,PosCrt+SUBSTRINGOFFSET,(LenCrt-(PosCrt+SUBSTRINGOFFSET)),DecimalStr);
	strcpy(NumberPtr,NumberStr);
	strcpy(DecimalPtr,DecimalStr);
}
