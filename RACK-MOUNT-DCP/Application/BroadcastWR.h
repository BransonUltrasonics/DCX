/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BroadcastWR.h_v   1.1   22 Sep 2016 11:48:42   EGUAJARDO  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BroadcastWR.h_v  $
 * 
 *    Rev 1.1   22 Sep 2016 11:48:42   EGUAJARDO
 * Applied coding standard.
 * 
 *    Rev 1.0   15 Sep 2016 17:26:18   EGUAJARDO
 * Initial revision.
 * 
*/

#ifndef BroadcastWR_H_
#define BroadcastWR_H_

#include "portable.h"
#include "Task.h"
#include "UtilsWR.h"

#define BROADCAST_WR_STACKSIZE 1024
#define BROADCAST_WR_BUFFERSIZE 256
#define BROADCAST_WR_PORT			83
#define BROADCASTPERMISSION		1
#define QUEUELENGTH					1
#define EQUALSGN						'='
#define TIMESTRLENGTH				20
#define WELDFREQFACTOR				1000

enum WRCommand
{
	wrOpenConnection = 1,
	wrCloseConnection,
	wrStartSending =	1,
	wrStopSending,
	wrKeepAlive = 1,
};


typedef BOOLEAN (*WRCmdHandler)(STR * ParamStrPtr, WRCommand Cmd);
struct WRCommandList {
	WRCmdHandler handler;
};




class BroadcastWR: public Task  {
public:
	BroadcastWR(SINT32 Eth);
	virtual ~BroadcastWR();
	static BOOLEAN EstablishConnection (STR * ParamStrPtr, WRCommand Cmd);
	static BOOLEAN StartSendResults (STR * ParamStrPtr,WRCommand Cmd);
	static BOOLEAN KeepAlive(STR * ParamStrPtr,WRCommand Cmd);
	static void Parser(STR* ReceiveStringPtr);
	static void SendWR();
private:
	SINT32 Eth;
protected:
	void Run();

};

#endif /* BroadcastWR_H_ */
