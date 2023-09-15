// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/RTOS/FIFO.h_v   1.1   24 Jul 2015 08:44:50   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines a FIFO buffer of arbitrary data types (integers,
// characters, object pointers etc). FIFOs are used for one-way task-to-task
// communication.

#ifndef FIFO_H_
#define FIFO_H_

#include "Task.h"
#include "ArchDefs.h"
#include "DebugNew.h"
#include "string.h"

class FifoBase
{
public:
	/**
	 * GetCount: Get entry count in the FIFO.
	 */
	short GetCount() const
	{
		return count;
	}
	const char * GetName() const
	{
		return fifoName;
	}
protected:
	FifoBase(int itemcount, int itemsize, const char * name);
	bool WriteTest(Task * writer, int timeout);
	void WriteCleanup();
	bool ReadTest(Task * writer, int timeout);
	void ReadCleanup();

	/** length: The length of the buffer, in objects. */
	short maxCount;
	/** count: Object count in the buffer. */
	volatile short count;
	/** read: Read index. */
	short read;
	/** write: Write index. */
	short write;
	/** waiting: Task waiting for input if buffer is empty.
	 * @deprecated Currently there can be only one waiting task.
	 */
	Task * waiting;
	/** fifoName: Name of the FIFO. */
	const char * fifoName;
};

/**
 * class Fifo: A template class that implements First-In-First-Out buffers of
 * any type. Buffer allows writing data into the buffer and reading from the
 * buffer in same order as it was written. Current implementation allows only
 * one task to read while waiting for timeout. Multiple tasks can write into
 * the buffer at 'same' time, they will all wait for some space in the buffer
 * to become available, then highest priority task will write first and lower
 * priority tasks will have their turn at it.
 */
template<class T> class Fifo: public FifoBase
{
public:
	/**
	 * Fifo: Constructor, allocates memory for the data buffer and
	 * initializes other fields.
	 * @param size Fifo size in objects.
	 */
	Fifo(int size, const char * name) :
		FifoBase(size, sizeof(T), name)
	{
		buffer = new T[maxCount];
	}
	/**
	 * ~Fifo: Clean up when object is being destroyed. Resume the task that is
	 * waiting for the event on it, and then delete the buffer.
	 * @deprecated Waiting task will not know what happened when Fifo is deleted.
	 */
	~Fifo()
	{
		if (waiting)
			waiting->Wakeup();
		delete buffer;
	}
	/**
	 * Write: Write a data element into Fifo structure, with optional
	 * timeout. Timeout 0 means return immediately if buffer is full,
	 * returning <b>false</b>. Timeout -1 means wait indefinitely.<p>
	 * CANBUFFER (16 bytes structure) writing with no timeout takes 2uS.
	 * @param writer Task that tries to write into the fifo buffer.
	 * @param data Data to put into the fifo.
	 * @param timeout Timeout in clock ticks.
	 * @return <b>true</b> if data was written.
	 */
	bool Write(Task * writer, T data, int timeout)
	{
		CriticalSection s;
		if (!WriteTest(writer, timeout))
			return false;
		// not full
		// if task is waiting, we need to set 'waiting' field to 0
		// before we resume the 'waiting' task, ot we may never get
		// a chance to get pointers straight in here...
		// this is why we use 'resume' pointer
		//		buffer[write] = data;
		memcpy(&buffer[write], &data, sizeof(data));
		WriteCleanup();
		return true;
	}
	/**
	 * Write: Write a data element into Fifo structure, with optional
	 * timeout. Timeout 0 means return immediately if buffer is full,
	 * returning <b>false</b>. Timeout -1 means wait indefinitely.<p>
	 * @param data Data to put into the fifo.
	 * @param timeout Timeout in milliseconds, or -1.
	 * @return <b>true</b> if data was written.
	 */
	bool Write(T data, int timeoutMs = -1)
	{
		if (timeoutMs == -1)
			return Write(0, data, -1);
		return Write(0, data, Task::MsToTicks(timeoutMs));
	}

	/**
	 * WriteRetry: Writes once and retries if the first write does not succeed.
	 * @param writer Writing task
	 * @param data Data to put into fifo
	 * @param timeout Timeout in milliseconds. Cannot use infinite wait here.
	 * @return true if data is written on one of the two write attempts
	 */
	bool WriteRetry(Task * writer, T data, int timeoutMs)
	{
		if (!Write(writer, data, 0))
			if(Write(writer, data, Task::MsToTicks(timeoutMs)))
				return true;
			else
				return false;
		else
			return true;
	}
	/**
	 * Read: Read an object from the FIFO buffer.<p>
	 * CANBUFFER (16 bytes) reading takes about 1.35uS.
	 * @param reader Reading task, may be suspended while reading if no
	 * data is available.
	 * @param data Buffer where data is stored after being read from the
	 * buffer.
	 * @param timeout Timeout in timer ticks after which reader task
	 * should give up. Timeout 0 means instant failure if buffer is empty,
	 * -1 will wait indefinitely.
	 * @return bool <b>true</b> if data was read.
	 */
	bool Read(Task * reader, T & data, int timeout)
	{
		CriticalSection s;
		if (!ReadTest(reader, timeout))
			return false;
		memcpy(&data, &buffer[read], sizeof(data));
		ReadCleanup();
		return true;
	}
	/**
	 * Read: Read an object from the FIFO buffer.<p>
	 * @param data Buffer where data is stored after being read from the
	 * buffer.
	 * @param timeout Timeout in milliseconds after which reader task
	 * should give up. Timeout 0 means instant failure if buffer is empty,
	 * -1 will wait indefinitely.
	 * @return bool <b>true</b> if data was read.
	 */
	bool Read(T & data, int timeoutMs = -1)
	{
		if (timeoutMs == -1)
			return Read(0, data, -1);
		return Read(0, data, Task::MsToTicks(timeoutMs));
	}

protected:
	/** buffer: Dynamically allocated buffer for objects. */
	T * buffer;
};

#endif
