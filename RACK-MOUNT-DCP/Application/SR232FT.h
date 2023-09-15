/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SR232FT.h_v   1.1   07 Jun 2016 12:01:18   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SR232FT.h_v  $
 * 
 *    Rev 1.1   07 Jun 2016 12:01:18   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   06 Jun 2016 15:41:22   GButron
 * Initial revision.
*/

#ifndef SR232FT_H_
#define SR232FT_H_

#include <stdlib.h>
#include "portable.h"
#include "UtilsFT.h"
#include "CommandsFT.h"
#include "TerminalFT.h"
#include "Task.h"
#include "RTOS.h"

#define SR232_FT_STACKSIZE                                     3072
#define SR232_FT_INPUT_SIZE                                      30
#define SR232_FT_OUTPUT_SIZE                                  80*24

class SR232FT : public Task {
public:
	STR InputStr[SR232_FT_INPUT_SIZE];
	STR OutPutStr[SR232_FT_OUTPUT_SIZE];
	SR232FT();
	virtual ~SR232FT();
private:
protected:
	void Run(void);
};
#endif /* SR232FT_H_ */
