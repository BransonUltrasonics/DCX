/*
 * FlashM29W320E.h
 *
 *  Created on: Nov 29, 2010
 *      Author: atali
 */

#ifndef FLASHM29W320E_H_
#define FLASHM29W320E_H_

#include "FlashAMD.h"

class FlashM29W320ET: public FlashAMD
{
public:
	FlashM29W320ET(unsigned int base, int bits);
protected:
	virtual int GetSectorSize(int offset);
};

class FlashM29W320EB: public FlashAMD
{
public:
	FlashM29W320EB(unsigned int base, int bits);
protected:
	virtual int GetSectorSize(int offset);
};

#endif
