/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SR232FT.cpp_v   1.1   07 Jun 2016 11:59:10   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SR232FT.cpp_v  $
 * 
 *    Rev 1.1   07 Jun 2016 11:59:10   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   06 Jun 2016 15:33:12   GButron
 * Initial revision.
*/

#include "SR232FT.h"

#define SR232FTTMR                               3000

/*  SR232FT::SR232FT():Task("SR232FT", SR232_FT_STACKSIZE)
 *
 *  Purpose:
 *   	 This is the constructor of SR232FT class.
 *
 *  Entry condition:
 *		 Intf - Ethernet interface the task is running on.
 *
 *  Exit condition:
 *  	 None.
 */
SR232FT::SR232FT():Task("SR232FT", SR232_FT_STACKSIZE)
{
	// TODO Auto-generated constructor stub
	memset(InputStr,0,COMMAND_INPUT_SIZE);
	memset(OutPutStr,0,COMMAND_OUTPUT_SIZE);
}


/*  SR232FT::~SR232FT()
 *
 *  Purpose:
 *   	 This is the destructor of SR232FT class.
 *
 *  Entry condition:
 *		 None.
 *
 *  Exit condition:
 *  	 None.
 */
SR232FT::~SR232FT()
{
	// TODO Auto-generated destructor stub
}


/******************************************************************************
*  void SR232FT::Run(void)                                                    *
*                                                                             *
*  Purpose:                                                                   *
*     This function is listening to the RS232 port and executes a command     *
*                                                                             *
*  Entry condition:                                                           *
*     None                                                                    *
*                                                                             *
*  Exit condition:                                                            *
*     None                                                                    *
******************************************************************************/
void SR232FT::Run(void)
{
   UINT8 CountCrt = 0;
   BOOLEAN CommandOkFlag = FALSE;

   DelayMs(SR232FTTMR);
   TerminalFT::Clrscr();
   while(TRUE)
   {
	   UtilsFT::Clean(InputStr);
	   TerminalFT::Readln(InputStr,SR232_FT_INPUT_SIZE);
	   if(!UtilsFT::IsEmpty(InputStr)) {
	      if(strcmp(InputStr,"CR")==0){ //clear the screen
		      TerminalFT::Clrscr();
	      }
	      else if(strcmp(InputStr,"?")==0) { //list commands
		      UtilsFT::Clean(OutPutStr);
	         strcpy(OutPutStr,"CR Clear Screen");
	         TerminalFT::Writeln(OutPutStr);
		      for(CountCrt=0; CountCrt<FT_ARRAY_SIZE; CountCrt++) {
			      UtilsFT::Clean(OutPutStr);
			      UtilsFT::Clean(InputStr);
			      sprintf(InputStr,"HE %d",CountCrt);
			      CommandOkFlag=CommandsFT::Execute(InputStr,OutPutStr);
	    	      if(!UtilsFT::IsEmpty(OutPutStr))
	    	      	TerminalFT::Writeln(OutPutStr);
		         }
	      }
	      else
	      { //execute ft cmd
	         UtilsFT::Clean(OutPutStr);
	         CommandOkFlag=CommandsFT::Execute(InputStr,OutPutStr);
	         TerminalFT::Writeln(OutPutStr);
	      }
	   }
   }
}
