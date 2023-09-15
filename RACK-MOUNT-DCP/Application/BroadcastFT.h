/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BroadcastFT.h_v   1.1   07 Jun 2016 11:35:26   GButron  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/BroadcastFT.h_v  $
 * 
 *    Rev 1.1   07 Jun 2016 11:35:26   GButron
 * Old revisions removed
 * 
 *    Rev 1.0   06 Jun 2016 13:04:58   GButron
 * Initial revision.
*/

#ifndef BROADCASTFT_H_
#define BROADCASTFT_H_

#include "portable.h"
#include "Task.h"
#include "UtilsFT.h"

#define BROADCAST_FT_STACKSIZE                                  1024
#define BROADCAST_FT_BUFFERSIZE                                  256
#define BROADCAST_FT_PORT                                         82

class BroadcastFT: public Task {
public:
	BroadcastFT(UINT8 Eth);
	virtual ~BroadcastFT();
private:
	UINT8 Eth;
protected:
	void Run(void);
};
#endif /* BROADCASTFT_H_ */
