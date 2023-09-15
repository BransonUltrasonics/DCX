/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/TerminalFT.cpp_v   1.1   07 Jun 2016 12:04:42   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/TerminalFT.cpp_v  $
 * 
 *    Rev 1.1   07 Jun 2016 12:04:42   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   07 Jun 2016 08:29:08   GButron
 * Initial revision.
 */

#include "TerminalFT.h"
#include "ReceiveCommandHandler.h"

#define UNITOFSUBTRACTION                        1

UINT16 TerminalFT::Row = 0;
UINT16 TerminalFT::Col = 0;
const STR TerminalFT::PromptStr[]={'F','T','>','\0'};

/************************************************************************************************
*  void TerminalFT::Clrscr(void)                                                                *
*                                                                                               *
*  Purpose:                                                                                     *
*     This function clears the terminal screen                                                  *
*                                                                                               *
*  Entry condition:                                                                             *
*     None                                                                                      *
*                                                                                               *
*  Exit condition:                                                                              *
*     None                                                                                      *
************************************************************************************************/
void TerminalFT::Clrscr(void)
{
	STR ClearingStringStr []  = "\033[2J\033[2J";

	Row = 0;
   Col = 0;
   pprintf(ClearingStringStr);
   GotoXY(Row,Col);
}


/************************************************************************************************
*  void TerminalFT::GotoXY(SINT32 Column,SINT32 Row)                                            *
*                                                                                               *
*  Purpose:                                                                                     *
*     This function prints out a row and column position                                        *
*                                                                                               *
*  Entry condition:                                                                             *
*     Column and Row                                                                            *
*                                                                                               *
*  Exit condition:                                                                              *
*     None                                                                                      *
************************************************************************************************/
void TerminalFT::GotoXY(SINT32 Column,SINT32 Row)
{
	STR CursorHomePositionStr []  = "\033[%d;%dH";

   pprintf(CursorHomePositionStr,Column,Row);
}


/************************************************************************************************
*  void TerminalFT::Readln(STR *ValuePtr,UINT8 Lenght)                                          *
*                                                                                               *
*  Purpose:                                                                                     *
*     This function reads the next character from the standard input                            *
*                                                                                               *
*  Entry condition:                                                                             *
*     ValuePtr - Object reference to get the input string                                       *
*     Length -  RS232 FT input size                                                             *
*                                                                                               *
*  Exit condition:                                                                              *
*     None                                                                                      *
************************************************************************************************/
void TerminalFT::Readln(STR *ValuePtr,UINT8 Lenght)
{
	BOOLEAN ExitInputFlag = FALSE;
	BYTE Letter ='\0';

	Col = TERMINALFT_PROMPT_SIZE;
	Row++;
	pprintf(PromptStr);
	GotoXY(Row,Col);
	while(ExitInputFlag == FALSE)
	{
	   Letter=getchar();
	   if(Letter>=FIRSTLETTEROFTHEALPHABETLOWERCASE && Letter<=LASTLETTEROFTHEALPHABETLOWERCASE)
	   	Letter=Letter-UPERCASECONVERSION; //Upper Case
	   switch (Letter) {
	      case TERMINALFT_KEY_ENTER:
	         Col=0;
	         GotoXY(Row,Col);
	         pprintf("FT>%s\n",ValuePtr);
	         ExitInputFlag=TRUE;
	         break;
	      case TERMINALFT_KEY_BACKSPACE:
	  	      if(Col<=TERMINALFT_PROMPT_SIZE) {
	  	         Col=TERMINALFT_PROMPT_SIZE;
	  	      }
	  	      else {
	  		      Col--;
	  	      }
	  	      Letter=' ';
	   	   GotoXY(Row,Col);
	         pprintf("%c",Letter);
	         Letter='\0';
	         ValuePtr[Col-TERMINALFT_PROMPT_SIZE]=Letter;
	         GotoXY(Row,Col);
	         break;
	         default:
	      if(Col>=Lenght-UNITOFSUBTRACTION) {
	         Letter=' ';
	    	   ValuePtr[Col-TERMINALFT_PROMPT_SIZE]=Letter;
	         pprintf("%c",Letter);
	         Col=Lenght-UNITOFSUBTRACTION;
	      }
	      else {
	         ValuePtr[Col-TERMINALFT_PROMPT_SIZE]=Letter;
	         pprintf("%c",Letter);
	         Col++;
	      }
	      GotoXY(Row,Col);
	   }
	}
}


/************************************************************************************************
*  void TerminalFT::Writeln(STR *ValuePtr)                                                      *
*                                                                                               *
*  Purpose:                                                                                     *
*     This function writes out a terminal response                                              *
*                                                                                               *
*  Entry condition:                                                                             *
*     STR - Object reference to get the input string                                            *
*                                                                                               *
*                                                                                               *
*  Exit condition:                                                                              *
*     None                                                                                      *
************************************************************************************************/
void TerminalFT::Writeln(STR *ValuePtr)
{
   UINT16 CountCtr = 0;

   Row++;
   Col = 0;
   GotoXY(Row,Col);
   pprintf("%s\n",ValuePtr);
   while(ValuePtr[CountCtr]!='\0')
   {
      if(ValuePtr[CountCtr]=='\n') {
		   Row++;
	      GotoXY(Row,Col);
	   }
      CountCtr++;
  }
}
