/*
 * KSZ8863RLL.h
 *
 *  Created on: Aug 13, 2012
 *      Author: RJamloki
 */

#ifndef KSZ8863RLL_H_
#define KSZ8863RLL_H_

#include "MII.h"

class KSZ8863RLL: public MII
{
	enum DeviceMiiRegisters
	{
		//registers specific to KSZ8863RLL

	};
public:
	KSZ8863RLL(int chan, int addr);
	virtual ~KSZ8863RLL();
	virtual bool Verify();
	virtual bool GetLinkStatus();
	virtual bool ReadLinkStatus();
};

#endif /* KSZ8863RLL_H_ */
