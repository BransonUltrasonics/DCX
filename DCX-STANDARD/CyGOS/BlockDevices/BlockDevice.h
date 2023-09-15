// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/BlockDevices/BlockDevice.h_v   1.0   29 Oct 2010 10:39:14   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares generic block driver object.

#ifndef BLOCKDEVICE_H_
#define BLOCKDEVICE_H_

#include "ArchDefs.h"
#include "FIFO.h"
#include "Task.h"

class CacheBlock;
class FileSystemManager;

class BlockDevice: public Task
{
	/**
	 * enum MessageCode: Message codes for the block device task.1
	 */
	enum MessageCode
	{
		/**
		 * Read: Read a sector from the device. Sector will be cached.
		 */
		Read,
		/**
		 * Write: Write a sector to the device. Sector will be cached.
		 */
		Write,
		/**
		 * Detect: Try to detect the drive. If drive is non-removable, this
		 * message is still periodically sent to the device task.
		 */
		Detect,
		/**
		 * Flush: Flush a single cache block. Message is sent from the Tick()
		 * function.
		 */
		Flush,
		/**
		 * FlushAll: Flush all cache blocks. Message is sent from other tasks
		 * when disk data needs to be synchronized.
		 */
		FlushAll,
	};
	/**
	 * struct Message: Command message sent from other tasks to this task
	 * message queue.
	 */
	struct Message
	{
		/**
		 * code: Message code: read, write, detect, flush etc.
		 */
		MessageCode code;
		/**
		 * sector: Sector number for read or write. In case of cache flush this
		 * field contains sector index in cache.
		 */
		int sector;
		/**
		 * buffer: Sector data buffer for read or write.
		 */
		uint8 * buffer;
		/**
		 * result: Queue receives error code when message is completely
		 * processed. Calling task must wait for that queue before continuing.
		 * Queue is temporarily allocated on caller task stack, so if timeout
		 * is used to read this queue, block device task may write into the
		 * queue after the queue is released from the stack.
		 * TODO This pointer can be 0 if message comes from block device tick
		 * handler!
		 */
		Fifo<int> * result;
		/**
		 * block: Cache block to flush if message was sent from Tick() function.
		 * Full cache flush is implemented through separate message code without
		 * using block pointers.
		 */
		CacheBlock * block;
	};
	bool cacheEnabled;
public:
	BlockDevice(char * name, int stack, bool caching = true);
	virtual ~BlockDevice();
	/**
	 * ReadDiskSector: Read the physical sector on the drive.
	 * @param sector Sector number to read.
	 * @param data Data buffer where to put the sector data.
	 * @return int Error code or 0 if no errors. Possible errors are listed in
	 * errno.h
	 */
	int ReadSector(int sector, void * buffer);
	/**
	 * WriteDiskSector: Write the physical sector to the drive.
	 * @param sector Sector number to write.
	 * @param data Data buffer where to get the sector data.
	 * @return int Error code or 0 if no errors. Possible errors are listed in
	 * errno.h
	 */
	int WriteSector(int sector, void * buffer);
	/**
	 * GetSectorSize: Return sector size in bytes. Mostly 512, but some few
	 * devices might have more than that, like large sector flash chips, for
	 * example.
	 * @return int Sector size in bytes.
	 */
	virtual int GetSectorSize() const
	{
		return 512;
	}
	virtual int GetSectorCount() const
	{
		return -1;
	}
	/**
	 * GetBlockSize: Cache block size for the device.
	 * @return int Cache block size, in sectors.
	 */
	virtual int GetBlockSize() const = 0;
	int FlushCache();
	virtual bool IsPartitionedDevice() const = 0;
	virtual int Initialize(int size, const void * data) = 0;
	virtual int Remove() = 0;
protected:
	virtual int ReadDiskSector(int sector, uint8 * data) = 0;
	virtual int WriteDiskSector(int sector, uint8 * data) = 0;
	void Run();
	void Tick();
	void ReadSector(Message & message);
	void WriteSector(Message & message);
	void FlushCacheBlock(Message & message);
	void FlushCache(Message & message);
	void ProbeDetectMedia();
	int FindCacheBlockContainingSector(int sector);
	int FlushCacheBlock(CacheBlock * block);
	int ReadSectorIntoCache(int index, int sector);
	void WriteSectorIntoCache(int index, int sector, uint8 * buffer);
	void MakeMRU(int blockIndex);
	/**
	 * DetectMedia: Try to detect the removable media that may be present in
	 * the drive.
	 * @return bool True if media is in the drive.
	 */
	virtual bool DetectMedia() = 0;
	/**
	 * MediaIsDetected: Return true if media is currently detected.
	 * @return bool true if media is currently detected.
	 */
	bool MediaIsDetected() const
	{
		return detected;
	}
	void InitCache(int bc, int bs);
	void DeleteCache();
	/**
	 * busy: Indicates that block device task is busy handling one message. To
	 * prevent message loss in input queue, Tick() will not send messages to
	 * flush cache buffers while task is busy. Also, this may help increase the
	 * performance for other tasks since their read and write messages will be
	 * sent to this task queue with less interference from cache flush messages.
	 * Cache will be eventually flushed anyway, so data integrity is maintained
	 * unless removable media is pulled out while dirty cache buffers exist.
	 */
	bool busy;
	/**
	 * queue: Message queue for the block device driver. All messages are
	 * handled through this queue, such as:
	 * <ol>
	 * <li>Delect - detect the drive with predefined intervals. Message is sent
	 * from timer tick function in this task.
	 * <li>Read or write - access to the disk sectors. Messages are sent mainly
	 * from file system tasks.
	 * </ol>
	 */
	Fifo<Message> queue;
	/**
	 * cache: The cache buffers table.
	 */
	CacheBlock ** cache;
	/**
	 * flushTimeout: Cache flush timeout in microseconds. This time is used to
	 * update cache buffer times when these buffers contain unwritten data.
	 */
	int flushTimeout;
	/**
	 * flushTimer: Cache flush timer. When this timer goes to zero, all cache
	 * buffer timeouts are updated and some sectors may be written back to the
	 * device.
	 */
	int flushTimer;
	/**
	 * cacheTimeout: Cache block timeout in microseconds. This value is set as
	 * a timeout for any cache block when the block is written to.
	 */
	int cacheTimeout;
	/**
	 * detectTimer: Media detect timer in microseconds.
	 */
	int detectTimer;
	/**
	 * detectTimeout: Timeout for media detection. Media probing is executed
	 * every this many microseconds.
	 */
	int detectTimeout;
	/**
	 * detected: When true, drive is detected. Initially the flag is false and
	 * detection code will set it to true.
	 */
	bool detected;
	/**
	 * sectorBuffer: Data buffer, one sector in size. Buffer is allocated in
	 * cache initialization routine.
	 */
	uint8 * sectorBuffer;
	/**
	 * cacheBlockSize: Cache block size in sectors. Size must be power of 2.
	 */
	int cacheBlockSize;
	/**
	 * cacheBlockCount: Cache block count.
	 */
	int cacheBlockCount;
};

#endif
