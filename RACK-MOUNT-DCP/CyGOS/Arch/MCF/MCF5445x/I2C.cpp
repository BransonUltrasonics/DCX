// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Arch/MCF/MCF5445x/I2C.cpp_v   1.0   18 Aug 2014 09:15:34   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements CPU-specific I2C driver.

#include "I2C.h"
#include "CPU.h"
#include "CPUdefs.h"
#include "ArchDefs.h"
#include "assert.h"
#include "DebugNew.h"
#include "RTOS.h"

I2C * I2C::i2c[1] = { 0 };
int I2CRetries;
const short I2C::divider[64] = { 28, 30, 34, 40, 44, 48, 56, 68, 80, 88, 104,
		128, 144, 160, 192, 240, 288, 320, 384, 480, 576, 640, 768, 960, 1152,
		1280, 1536, 1920, 2304, 2560, 3072, 3840, 20, 22, 24, 26, 28, 32, 36,
		40, 48, 56, 64, 72, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384,
		448, 512, 640, 768, 896, 1024, 1280, 1536, 1792, 2048 };

/**
 * I2C: Initialize the I2C driver with reasonable defaults.
 * @param instance I2C instance index, useful when processors have multiple I2C
 * interfaces.
 * @param address Slave address for this I2C interface. TODO For now slave mode
 * is not implemented.
 * @param intLevel Interrupt level to use for this I2C interface. TODO For now I2C
 * uses polling mose.
 * @param intPriority Interrupt priority within level.
 */
I2C::I2C(int instance, int address, int intLevel, int intPriority) :
	mutex("I2C")
{
	ASSERT(instance == 0);
	ASSERT(i2c[instance] == 0);
	i2c[instance] = this;
	slaveAddress = address;
	//	CPU::SetupHandler(InterruptHandler, intI2C, intLevel, intPriority);
	int par = MCF_PAD_PAR_FECI2C & 0xF0;
	par |= MCF_PAD_PAR_FECI2C_PAR_SCL_SCL; // enable I2C SCL pin
	par |= MCF_PAD_PAR_FECI2C_PAR_SDA_SDA; // enable I2C SDA pin
	MCF_PAD_PAR_FECI2C = par;
	// set divider to 200 so I2C runs at 400KHz, this should be good enough for
	// all devices. Divider is 240, so actual frequency is a bit less.
	MCF_I2C_I2FDR = MCF_I2C_I2FDR_IC(0x14);
	MCF_I2C_I2CR = MCF_I2C_I2CR_IEN;
	// enable I2C
	if (MCF_I2C_I2SR & MCF_I2C_I2SR_IBB)
	{
		printf("IBB on I2C set. Resetting I2C.\n");
		MCF_I2C_I2CR = 0;
		MCF_I2C_I2CR = MCF_I2C_I2CR_IEN | MCF_I2C_I2CR_MSTA;
		char dummy = MCF_I2C_I2DR;
		MCF_I2C_I2SR = 0;
		MCF_I2C_I2CR = 0;
		MCF_I2C_I2CR = MCF_I2C_I2CR_IEN;
		printf("%c",dummy);
	}
	pprintf("I2C initialized\n %d %d %d", instance,intPriority, intLevel);
}

/**
 * InterruptHandler: Static function calls interrupt handler function for
 * specific I2C interface, depending on interrupt level. If interrupt level is
 * not valid (handler installed at wrong interrupt vector), function asserts.
 * @param level Interrupt level.
 * @param frame Interrupt stack frame pointer.
 */
void I2C::InterruptHandler(int level, void * frame)
{
	if (level == IntVector(intI2C))
		i2c[0]->InterruptHandler();
	else
	{
		ASSERT(0);
	   ASSERT(frame);
		//disable compiler warning
	}
}

/**
 * InterruptHandler: Handle interrupts for specific I2C interface. TODO No interrupts, polling mode only.
 */
void I2C::InterruptHandler()
{
}

/**
 * Read: Read data from I2C device. Multiple bytes are read, up to maxLength.
 * @param address Device address on I2C bus.
 * @param subAddress
 * @param subAddrLen
 * @param data Buffer where to place the read data.
 * @param dataLen Buffer length.
 */
void I2C::Read(int address, int subAddress, int subAddrLen,
		unsigned char * data, int &dataLen, int MaxRetries)
{
	do
	{
		Disable();
		Enable();
		Transmit();
		MasterAckOff();
		MasterStart();
		MCF_I2C_I2DR = address & 0xFE;
		WaitForInterrupt();
		NoRepeatStart();
		I2CRetries++;
		if(I2CRetries >= MaxRetries)
		{
			dataLen = -1;
			return;
		}
	} while (MCF_I2C_I2SR & MCF_I2C_I2SR_RXAK);
	int shift = (subAddrLen - 1) * 8;
	for (int i = 0; i < subAddrLen; i++)
	{
		MCF_I2C_I2DR = subAddress >> shift;
		subAddress <<= 8;
		WaitForInterrupt();
	}
	RepeatStart();
	MCF_I2C_I2DR = address | 0x01;
	WaitForInterrupt();
	Receive();
	NoRepeatStart();
	MasterAckOn();
	*data = MCF_I2C_I2DR;
	WaitForInterrupt();
	for (int i = 0; i < dataLen; i++)
	{
		if (i == (dataLen - 1))
			MasterAckOff();
		*data++ = MCF_I2C_I2DR;
		WaitForInterrupt();
	}
	MasterStop();
	Disable();
}

/**
 * Write: Write data to an I2C device.
 * @param address Device address on I2C bus.
 * @param subAddress
 * @param subAddrLen
 * @param data Buffer containing data to send to the device.
 * @param dataLen Data length to write.
 */
void I2C::Write(int address, int subAddress, int subAddrLen,
		const unsigned char * data, int dataLen)
{
	do
	{
		Disable();
		Enable();
		Transmit();
		MasterAckOff();
		MasterStart();
		MCF_I2C_I2DR = address & 0xFE;
		WaitForInterrupt();
		NoRepeatStart();
	} while (MCF_I2C_I2SR & MCF_I2C_I2SR_RXAK);
	int shift = (subAddrLen - 1) * 8;
	for (int i = 0; i < subAddrLen; i++)
	{
		MCF_I2C_I2DR = subAddress >> shift;
		subAddress <<= 8;
		WaitForInterrupt();
	}
	for (int i = 0; i < dataLen; i++)
	{
		MCF_I2C_I2DR = *data++;
		WaitForInterrupt();
	}
	MasterStop();
	//Disable();
}

/**
 * WaitForInterrupt: Wait for device to generate interrupt when data transfer is
 * complete. TODO For now uses only polling mode, with infinite timeout.
 * @return bool True if interrupt was detected (TODO Always true for now).
 */
bool I2C::WaitForInterrupt()
{
	while (!(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF))
	{
	}
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;
	return true;
}

/**
 * Acquire: Acquire the I2C interface. Each read or write access on higher level
 * must be safeguarded by Acquire/Release function calls because I2C bus is
 * shared and multiple tasks can preempt each other and drive I2C interface
 * out of sync.
 * @param timeout Timeout to wait for the acquire mutex, in RTOS ticks.
 * @return bool True if I2C was acquired within specified timeout. If function
 * is called before RTOS starts, there is no contention between tasks, so
 * function returns true without acquiring the mutex.
 */
bool I2C::Acquire(int timeout)
{
	if (RTOS::IsRunning())
		return mutex.Acquire(RTOS::GetCurrentTask(), timeout);
	return true;
}

/**
 * Release: Release the I2C interface. If function is called before RTOS starts,
 * there is no contention between tasks, so function returns without releasing
 * the mutex because it was not acquired in the Acquire function.
 */
void I2C::Release()
{
	if (RTOS::IsRunning())
		mutex.Release(RTOS::GetCurrentTask());
}
