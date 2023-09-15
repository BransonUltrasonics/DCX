/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/TerminalFT.h_v   1.1   07 Jun 2016 12:06:56   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/TerminalFT.h_v  $
 * 
 *    Rev 1.1   07 Jun 2016 12:06:56   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   07 Jun 2016 08:29:44   GButron
 * Initial revision.
*/

#ifndef TERMINALFT_H_
#define TERMINALFT_H_

#include "portable.h"
#include "UtilsFT.h"
#include "RTOS.h"

#define TERMINALFT_PROMPT_SIZE                            4
#define TERMINALFT_KEY_ENTER                              13
#define TERMINALFT_KEY_BACKSPACE                          8

class TerminalFT {
public:
	static void Clrscr(void);
	static void GotoXY(SINT32 Column,SINT32 Row);
	static void Readln(STR *ValuePtr,UINT8 Lenght);
	static void Writeln(STR *ValuePtr);
private:
	static UINT16 Row;
	static UINT16 Col;
	static const STR PromptStr[TERMINALFT_PROMPT_SIZE];
};
#endif /* TERMINALFT_H_ */
