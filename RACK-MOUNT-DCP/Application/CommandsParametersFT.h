/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/CommandsParametersFT.h_v   1.1   07 Jun 2016 11:50:14   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/CommandsParametersFT.h_v  $
 * 
 *    Rev 1.1   07 Jun 2016 11:50:14   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   06 Jun 2016 13:23:40   GButron
 * Initial revision.
*/

#ifndef COMMANDSPARAMETERSFT_H_
#define COMMANDSPARAMETERSFT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "portable.h"
#include "UtilsFT.h"

#define PARAMETERS_ITEM_SIZE                                    256
#define COMMAND_PARAMETERS_ARRAY_SIZE                            10
#define COMMAND_PARAMETERS_TOKEN_SIZE                           256

class CommandsParametersFT {
private:
	STR    ItemsStr[COMMAND_PARAMETERS_ARRAY_SIZE][COMMAND_PARAMETERS_TOKEN_SIZE];
	SINT16 Size;
	void Zap(void);
public:
	CommandsParametersFT(void);
	virtual ~CommandsParametersFT(void);
	void Display(void);
	void Add(STR *ValuePtr);
	void Erase(SINT16 Index);
	void Get(SINT16 Index,STR *ValuePtr);
	void Set(SINT16 Index,STR *ValuePtr);
	void Clear(void);
	SINT16 Length(void);
};

#endif /* COMMANDSPARAMETERSFT_H_ */
