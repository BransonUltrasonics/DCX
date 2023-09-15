/*
 * FlashM29W128G.h
 *
 *  Created on: Aug 9, 2012
 *      Author: RJamloki
 */

#ifndef FLASHM29W128G_H_
#define FLASHM29W128G_H_

#include "FlashAMD.h"

class FlashM29W128GL: public FlashAMD
{
public:
	FlashM29W128GL(unsigned int base, int bits);
protected:
	virtual int GetSectorSize(int offset);
	virtual bool Program(void * flash, void * data, int length,
				FlashCallback cb = 0);
	virtual bool Erase(void * flash, int length, FlashCallback cb = 0);
	virtual unsigned int ReadFlashType();
};

#endif /* FLASHM29W128G_H_ */
