// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Arch/MCF/MCF5445x/I2C.h_v   1.0   18 Aug 2014 09:15:34   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares CPU-specific I2C driver.

#ifndef I2C_H_
#define I2C_H_

#include "Mutex.h"
#include "CPUdefs.h"

/**
 * I2C: MCF5329 I2C interface driver. Interface provides write (N bytes), read
 * without specifying device-internal address (N bytes) and read from device-
 * internal address (1 byte at a time).
 */
class I2C
{
public:
	I2C(int instance, int address, int intLevel, int intPriority);
	void Read(int address, int subAddress, int subAddrLen,
			unsigned char * data, int &dataLen, int MaxRetries);
	void Write(int address, int subAddress, int subAddrLen,
			const unsigned char * data, int dataLen);
	void WriteByteReg(int addr, int subAddr, int value)
	{
		unsigned char ch = value;
		Write(addr, subAddr, 1, &ch, 1);
	}
	int ReadByteReg(int addr, int subAddr)
	{
		unsigned char ch;
		int len=1;
		Read(addr, subAddr, 1, &ch, len,5);
		return ch;
	}
	bool Acquire(int timeout = -1);
	void Release();
protected:
	static void InterruptHandler(int level, void * frame);
	void Disable()
	{
		MCF_I2C_I2CR = 0;
	}
	void Enable()
	{
		MCF_I2C_I2CR |= MCF_I2C_I2CR_IEN;// | I2C_I2CR_IIEN;
	}
	void Transmit()
	{
		MCF_I2C_I2CR |= MCF_I2C_I2CR_MTX;
	}
	void MasterStart()
	{
		MCF_I2C_I2CR |= MCF_I2C_I2CR_MSTA;
	}
	void MasterStop()
	{
		MCF_I2C_I2CR &= ~MCF_I2C_I2CR_MSTA;
	}
	void NoRepeatStart()
	{
		MCF_I2C_I2CR &= ~MCF_I2C_I2CR_RSTA;
	}
	void RepeatStart()
	{
		MCF_I2C_I2CR |= MCF_I2C_I2CR_RSTA;
	}
	void Receive()
	{
		MCF_I2C_I2CR &= ~MCF_I2C_I2CR_MTX;
	}
	void MasterAckOff()
	{
		MCF_I2C_I2CR |= MCF_I2C_I2CR_TXAK;
	}
	void MasterAckOn()
	{
		MCF_I2C_I2CR &= ~MCF_I2C_I2CR_TXAK;
	}
	void InterruptHandler();
	bool WaitForInterrupt();
	/**
	 * Mutex: Mutually-exclusive lock to the I2C driver. Only one task at a
	 * time can access the I2C driver, all others will wait. When current
	 * owner task releases the mutex, highest priority waiting task takes the
	 * mutex and gains access to the driver. This is necessary so that one
	 * transfer will not be preempted with transfer from other task because
	 * I2C settings for each peripheral device are most likely different, and
	 * settings must be maintained until the mutex owner task has completed all
	 * necessary transfers.<p>
	 * Because of this, high priority task may be delayed by a low priority
	 * task that has long transfers active. Since each transfer over I2C is
	 * fairly short, higher priority tasks should have a fair chance to get to
	 * transfer data over I2C.<p>
	 * I2C driver can be used without RTOS (TODO at least for now, in polling mode).
	 * In this case mutex functions are not called.
	 */
	Mutex mutex;
	/**
	 * slaveAddress: Address of this I2C interface as a slave.
	 */
	int slaveAddress;
	static I2C * i2c[1];
	static const short divider[];
};

#endif
