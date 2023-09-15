/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/CommandsFT.h_v   1.2   25 Jul 2016 12:10:24   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/CommandsFT.h_v  $
 * 
 *    Rev 1.2   25 Jul 2016 12:10:24   GButron
 * Prototype static void WriteReadFRAM () added
 * 
 *    Rev 1.1   07 Jun 2016 11:41:30   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   06 Jun 2016 13:16:06   GButron
 * Initial revision.
*/

#ifndef COMMANDSFT_H_
#define COMMANDSFT_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "portable.h"
#include "UtilsFT.h"
#include "CommandsParametersFT.h"
#include "BroadcastFT.h"

#define COMMAND_DEFINITION_SIZE                         256
#define COMMAND_INPUT_SIZE                              256
#define COMMAND_OUTPUT_SIZE                           80*24
#define PARSE_OUTPUT_SIZE                               256
#define PARSE_ARRAY_SIZE                                 13
#define FT_ARRAY_SIZE                                    90
#define BITPOSITIONS                                     24
#define TWOTHIRDBITPOSITIONS                             16
#define ONETHIRDBITPOSITIONS                              8
#define TWENTYFOURVOLTSPRESENT                         0x10

class CommandsFT {
public:
	typedef void (*CallBackFTPtr)(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	enum CommandType {
	   FT_ERROR=-1,
	   FT_NONE=0,
	   FT_COMMAND=1,
	   FT_HELP=2
	};
	enum CommandStatus {
	   FT_ON=1,
	   FT_OFF=0
	};
	struct DefinitionFT	{
	   STR NameStr[COMMAND_DEFINITION_SIZE];
	   STR DescriptionStr[COMMAND_DEFINITION_SIZE];
	   STR SyntaxStr[COMMAND_DEFINITION_SIZE];
	   UINT8 NumberParameters;
	   CallBackFTPtr CallBackFunction;
	};

	static DefinitionFT FT[FT_ARRAY_SIZE];
	static CommandType Type;
	static UINT8 Command;
	static STR MessageStr[PARSE_OUTPUT_SIZE];
   static CommandsParametersFT Parameters;
	static CommandStatus Status;
	static BOOLEAN Execute(STR *InputFtPtr,STR *OutputFtPtr);
	static UINT8 Priority;
	/* hardware commands */
	static void DCPFT09(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT10(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT11(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT12(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT13(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT14(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT15(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT41(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT46(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT59(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT64(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT71(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT72(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT73(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT75(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT76(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT77(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT78(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT79(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT80(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
	static void DCPFT81(CommandsParametersFT *ParametersPtr,STR **OutputFtPtr);
protected:
private:
	static const STR *ParseArrPtr[PARSE_ARRAY_SIZE];
	static BOOLEAN Parse(STR **FtPtr);
	static void TokenFill(STR *TokenPtr);
	static void WriteReadFRAM ();
};

#endif /* COMMANDSFT_H_ */
