// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/API/SPI.h_v   1.1   28 Jan 2015 05:47:42   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares generic SPI API.

#ifndef SPI_H_
#define SPI_H_

#include "Mutex.h"

class SPI
{
	friend class SpiLock;
public:
	virtual void EnableGPIO() = 0;
	virtual void EnableSPI(bool phase, int bits, int clock, bool polarity =
			true, bool lsb = false, uint32 DelayParams = 0) = 0;
	virtual void SetSignals(int clock, int output, int cs) = 0;
	virtual int ReadInput() = 0;
	virtual unsigned char ReadByte(unsigned char send, int chipSelect) = 0;
	virtual void SingleTransfer(int length, uint16 * data, int chipSelect,
			bool cont) = 0;
	virtual void LongWordTransfer(int length, uint8 * data, int maxLength,
			int chipSelect) = 0;
	virtual uint32 SetDelayParams(int, int, int, int,
			int, int){
	   return 0;
	}

	SPI() :
		mutex("SPI")
	{

		Speed = 0;
		nBits = 0;
		Lsb = false;
		Phase = false;
		Polarity = false;
	}
	/**
	 * Acquire: Acquire the QSPI device.
	 * @param timeout Timeout to acquire, in RTOS ticks, or -1 for infinite
	 * wait.
	 * @return bool true if QSPI was acquired in time.
	 */
	bool Lock(int timeout)
	{
		return mutex.Acquire(0, timeout);
	}
	/**
	 * Release: Release the QSPI driver.
	 */
	void Release()
	{
		mutex.Release(0);
	}
protected:
	/**
	 * Mutex: Mutually-exclusive lock to the SPI driver. Only one task at a
	 * time can access the SPI driver, all others will wait. When current
	 * owner task releases the mutex, highest priority waiting task takes the
	 * mutex and gains access to the driver. This is necessary so that one
	 * transfer will not be preempted with transfer from other task because
	 * SPI settings such as bit timing, clock phases, chip select states for
	 * each peripheral device are most likely different, and settings must be
	 * maintained until the mutex owner task has completed all necessary
	 * transfers.<p>
	 * Because of this, high priority task may be delayed by a low priority
	 * task that has long transfers active. System design should be such that
	 * transfers over SPI are small enough to give all tasks a fair chance to
	 * get to transfer data over SPI.
	 */
	Mutex mutex;
	//The clock of configured SPI chip
	int Speed;
	//transfer bits
	int nBits;
	//lsb
	bool Lsb;
	//phase
	bool Phase;
	//polarity
	bool Polarity;
	//flag to determine whether to use regular SPI functionality or some bit-bang
	bool spiflag;
	//
	unsigned int DelayParameters;
};

/**
 * class SpiLock: Helper task to acquire the SPI driver when object is
 * created, and release when deleted. This object will automatically release
 * the SPI driver once processing is complete in some class function.
 */
class SpiLock
{
public:
	/**
	 * SpiLock: Initialize the lock object. Constructor does not try to
	 * immediately acquire the driver because there is no way to return the
	 * acquire status from the constructor.
	 * @param q SPI driver.
	 */
	SpiLock(SPI * q)
	{
		spi = q;
		locked = false;
	}
	/**
	 * Lock: Lock the SPI driver to be used by current task.
	 * @param timeout Timeout to wait, in RTOS ticks, or -1 for infinite wait.
	 * @return bool true if SPI was locked in time.
	 */
	bool Lock(int timeout)
	{
		locked = spi->Lock(timeout);
		return locked;
	}
	/**
	 * UnLock: UnLock the SPI driver to be used by current task.
	 */
	void UnLock()
	{
		if (locked)
		{
			spi->Release();
			locked = false;
		}
	}
	/**
	 * ~SpiLock: Release the SPI driver if it was locked by this object.
	 */
	~SpiLock()
	{
		UnLock();
	}
protected:
	/**
	 * spi: SPI driver to lock.
	 */
	SPI * spi;
	/**
	 * locked: If true, this SPI was locked by this task.
	 */
	bool locked;
};

#endif
