/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/CommandsParametersFT.cpp_v   1.1   07 Jun 2016 11:46:38   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/CommandsParametersFT.cpp_v  $
 * 
 *    Rev 1.1   07 Jun 2016 11:46:38   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   06 Jun 2016 13:20:26   GButron
 * Initial revision.
*/

#include "CommandsParametersFT.h"

#define LENGTHOFFSET                                    1
#define ITEMSOFFSET                                     1

STR    ItemsStr[COMMAND_PARAMETERS_ARRAY_SIZE][COMMAND_PARAMETERS_TOKEN_SIZE] = {};

 /**************************************************************************************
 *  CommandsParametersFT::CommandsParametersFT(void)                                   *
 *                                                                                     *
 *  Purpose:                                                                           *
 *   	This is the constructor of CommandsParametersFT class.                           *
 *                                                                                     *
 *  Entry condition:                                                                   *
 *		Intf - Ethernet interface the task is running on.                                *
 *                                                                                     *
 *  Exit condition:                                                                    *
 *  	None.                                                                            *
 **************************************************************************************/
CommandsParametersFT::CommandsParametersFT(void)
{
   this->Size = 0;
   this->Zap();
}


 /**************************************************************************************
 *  CommandsParametersFT::~CommandsParametersFT(void)                                  *
 *                                                                                     *
 *  Purpose:                                                                           *
 *   	This is the destructor of CommandsParametersFT class                             *
 *                                                                                     *
 *  Entry condition:                                                                   *
 *		None                                                                             *
 *                                                                                     *
 *  Exit condition:                                                                    *
 *  	None                                                                             *
 ***************************************************************************************/
CommandsParametersFT::~CommandsParametersFT(void)
{
}


 /**************************************************************************************
 *  void CommandsParametersFT::Display                                                 *
 *                                                                                     *
 *  Purpose:                                                                           *
 *		This function display the list of array parameter allocated in the object class  *
 *                                                                                     *
 *  Entry condition:None                                                               *
 *                                                                                     *
 *  Exit condition:None                                                                *
 **************************************************************************************/
void CommandsParametersFT::Display(void)
{
   SINT16 IndexCtr = 0;

   for(IndexCtr = 0; IndexCtr<this->Size; ++IndexCtr) {
      printf("%s \n",this->ItemsStr[IndexCtr]);
   }
}


 /**************************************************************************************
 *  void CommandsParametersFT::Add(STR *ValuePtr)                                      *
 *                                                                                     *
 *  Purpose:                                                                           *
 *		This function add a parameter to the array list                                  *
 *                                                                                     *
 *  Entry condition:                                                                   *
 *     ValuePtr - String value of the parameter                                        *
 *                                                                                     *
 *  Exit condition:None                                                                *
 *                                                                                     *
 ***************************************************************************************/
void CommandsParametersFT::Add(STR *ValuePtr)
{
   SINT16 IndexCtr = this->Size;

   if(this->Size<COMMAND_PARAMETERS_ARRAY_SIZE) {
      strcpy(this->ItemsStr[IndexCtr],ValuePtr);
      IndexCtr++;
      this->Size = IndexCtr;
   }
   else {
	   printf("overflow \n");
   }
}


 /**************************************************************************************
 *  void CommandsParametersFT::Erase(SINT16 Index)                                     *
 *                                                                                     *
 *  Purpose:                                                                           *
 *		 This function erase a parameter from it index position                          *
 *                                                                                     *
 *  Entry condition:                                                                   *
 *     Index - Position of the index to be erase                                       *
 *                                                                                     *
 *  Exit condition:None                                                                *
 *                                                                                     *
 **************************************************************************************/
void CommandsParametersFT::Erase(SINT16 Index)
{
   SINT16 IndexCtr = 0;
   SINT16 LengthOldCtr = 0;
   SINT16 LengthNewCtr = 0;
   STR ItemsNewArrStr[COMMAND_PARAMETERS_ARRAY_SIZE][COMMAND_PARAMETERS_TOKEN_SIZE] = {};

   if(Index>=0 && Index<=this->Size) {
	   LengthOldCtr = this->Size;
	   LengthNewCtr = LengthOldCtr - LENGTHOFFSET;
	   for(IndexCtr=0; IndexCtr<COMMAND_PARAMETERS_ARRAY_SIZE; ++IndexCtr) {
	      UtilsFT::Clean(ItemsNewArrStr[IndexCtr]);
	   }
      for(IndexCtr=0; IndexCtr<Index; ++IndexCtr) {
	      strcpy(ItemsNewArrStr[IndexCtr],this->ItemsStr[IndexCtr]);
      }
      for(IndexCtr=Index; IndexCtr<LengthNewCtr; ++IndexCtr) {
	      strcpy(ItemsNewArrStr[IndexCtr],this->ItemsStr[IndexCtr+ITEMSOFFSET]);
      }
	   Zap();
	   for(IndexCtr=0; IndexCtr<LengthNewCtr; ++IndexCtr) {
	      strcpy(this->ItemsStr[IndexCtr],ItemsNewArrStr[IndexCtr]);
	   }
	   this->Size=LengthNewCtr;
   }
}


 /**************************************************************************************
 *  void CommandsParametersFT::Get(SINT16 IndexCtr,STR *ValuePtr)                      *
 *                                                                                     *
 *  Purpose:                                                                           *
 *		 This function get's a parameter according to it index position                  *
 *                                                                                     *
 *  Entry condition:                                                                   *
 *     Index - Position of the parameter                                               *
 *     Value - Value pointer to be return                                              *
 *                                                                                     *
 *  Exit condition:None                                                                *
 *                                                                                     *
 **************************************************************************************/
void CommandsParametersFT::Get(SINT16 IndexCtr,STR *ValuePtr)
{
   if(IndexCtr>=0 && IndexCtr<=this->Size) {
      strcpy(ValuePtr,this->ItemsStr[IndexCtr]);
   }
}


 /**************************************************************************************
 *  void CommandsParametersFT::Set(SINT16 IndexCtr,STR *ValuePtr)                      *
 *                                                                                     *
 *  Purpose:                                                                           *
 *		This function change the value of the parameter according of it index position   *
 *                                                                                     *
 *  Entry condition:                                                                   *
 *  Index - Position of the parameter                                                  *
 *  Value - New value pointer to be change                                             *
 *                                                                                     *
 *  Exit condition:None                                                                *
 **************************************************************************************/
void CommandsParametersFT::Set(SINT16 IndexCtr,STR *ValuePtr)
{
   if(IndexCtr>=0 && IndexCtr<=this->Size) {
      strcpy(this->ItemsStr[IndexCtr], ValuePtr);
   }
}


 /*******************************************************************************************
 *  void CommandsParametersFT::Clear(void)                                                  *
 *                                                                                          *
 *  Purpose:                                                                                *
 *		This function clear the array list of parameters and return the index position to 0   *
 *                                                                                          *
 *  Entry condition:None                                                                    *
 *                                                                                          *
 *  Exit condition:None                                                                     *
 *                                                                                          *
 *******************************************************************************************/
void CommandsParametersFT::Clear(void)
{
   this->Size=0;
   this->Zap();
}


/***************************************************************************************
 *  SINT16 CommandsParametersFT::Length(void)                                          *
 *                                                                                     *
 *  Purpose:                                                                           *
 *		This function gives the length of the array list parameters                      *
 *                                                                                     *
 *  Entry condition:None                                                               *
 *                                                                                     *
 *  Exit condition:None                                                                *
 *                                                                                     *
 **************************************************************************************/
SINT16 CommandsParametersFT::Length(void)
{
   return this->Size;
}


/***************************************************************************************
 *  void CommandsParametersFT::Zap(void)                                               *
 *                                                                                     *
 *  Purpose:                                                                           *
 *		This function clear the array list of parameters                                 *
 *                                                                                     *
 *  Entry condition:None                                                               *
 *                                                                                     *
 *  Exit condition:None                                                                *
 *                                                                                     *
 ***************************************************************************************/
void CommandsParametersFT::Zap(void)
{
   SINT16 IndexCtr = 0;

   for(IndexCtr=0; IndexCtr<COMMAND_PARAMETERS_ARRAY_SIZE; ++IndexCtr) {
      UtilsFT::Clean(this->ItemsStr[IndexCtr]);
   }
}
