// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/BlockDevices/BlockDevice.cpp_v   1.0   18 Aug 2014 09:16:14   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares generic block driver object.

#include "BlockDevice.h"
#include "CacheBlock.h"
#include "FileSystemManager.h"
#include "assert.h"
#include "errno.h"
#include "stdio.h"
#include "DebugNew.h"

BlockDevice::BlockDevice(char * name, int stack, bool caching) :
	Task(name, stack), queue(20, "BlkDev")
{
	detected = false;
	detectTimer = 0;
	cache = 0;
	flushTimeout = 100000; // 0.1s tick for all buffers
	flushTimer = 0;
	cacheTimeout = 1000000; // 1s timeout for all cache buffers
	busy = true;
	sectorBuffer = 0;
	cacheEnabled = caching;
}

/**
 * ~BlockDevice: Deinitialize the block device task object. Delete caches
 * without flushing. Caller must take care of forced flush the cache if media
 * is still present when the task is deleted.
 */
BlockDevice::~BlockDevice()
{
	if (cacheEnabled)
		DeleteCache();
}

/**
 * InitCache: Initialize the block device cache. All blocks are allocated and
 * set to unused. Function is called when new media is detected in the drive,
 * so cache configuration is always specific to the media parameters.
 * @param bc Cache block count.
 * @param bs Cache block size in sectors.
 */
void BlockDevice::InitCache(int bc, int bs)
{
	DeleteCache(); // just in case...
	cacheBlockSize = bs;
	cacheBlockCount = bc;
	// Verify that block size is power of 2. This is needed for determining
	// the beginning sector number in the block based on sector we access.
	ASSERT((cacheBlockSize & (cacheBlockSize - 1)) == 0);
	// Disable interrupts while we initialize cache, or Tick() could pick up
	// cache work-in-progress and start flushing non-existant blocks...
	CriticalSection s;
	cache = new CacheBlock *[cacheBlockCount];
	for (int i = 0; i < cacheBlockCount; i++)
	{
		cache[i] = new CacheBlock(cacheBlockSize, GetSectorSize());
	}
	delete sectorBuffer;
	sectorBuffer = new unsigned char[GetSectorSize()];
}

/**
 * DeleteCache: Delete all cache blocks. This function does not attempt to
 * flush any dirty blocks.
 */
void BlockDevice::DeleteCache()
{
	CriticalSection s;
	if (cache)
	{
		for (int i = 0; i < cacheBlockCount; i++)
		{
			delete cache[i];
		}
		delete cache;
		cache = 0;
	}
}

void BlockDevice::Tick()
{
	detectTimer += usecPerTick;
	if (detectTimer >= detectTimeout)
	{
		detectTimer -= detectTimeout;
		if(!detected){
			Message message;
			memset(&message, 0, sizeof(Message));
			message.code = Detect;
			queue.Write(0, message, 0);
		}
	}
//	if (cache)
//	{
//		flushTimer += usecPerTick;
//		if (flushTimer >= flushTimeout)
//		{
//			flushTimer -= flushTimeout;
//			bool queueIsFull = false;
//			for (int blockIndex = 0; blockIndex < cacheBlockCount; blockIndex++)
//			{
//				CacheBlock * block = cache[blockIndex];
//				if (block->IsDirty())
//				{
//					block->UpdateTimer(flushTimeout);
//					if (block->IsTimerExpired())
//					{
//						if ((!queueIsFull) && (!busy))
//						{
//							Message message;
//							memset(&message, 0, sizeof(Message));
//							message.code = Flush;
//							message.block = block;
//							// Result FIFO is 0 so we don't care about the
//							// errors in here. Not much we can do...
//							// TODO This is wrong, errors must be handled or
//							// at least reported!
//							message.result = 0;
//							if (!queue.Write(0, message, 0))
//								queueIsFull = true;
//						}
//					}
//				}
//			}
//		}
//	}
}

void BlockDevice::Run()
{
	for (;;)
	{
		Message message;
		{
			// Wait for messages while not busy and process them while busy so
			// that Tick() will not insert messages to flush caches while we
			// are busy working...
			//CriticalSection s;
			busy = false;
			queue.Read(this, message, -1);
			busy = true;
		}
		switch (message.code)
		{
		case Detect:
			ProbeDetectMedia();
			break;
		case Read:
			ReadSector(message);
			break;
		case Write:
			WriteSector(message);
			break;
		case Flush:
			FlushCacheBlock(message);
			break;
		case FlushAll:
			FlushCache(message);
			break;
		default:
			CRASH("Bad message code sent to BlkDev");
		}
	}
}

/**
 * ReadSector: Read a sector from the block device. This is the function to call
 * when a sector needs to be read. The function is executed from caller task
 * context. Function passes messages to the block device task to perform cache
 * management, and then it may indirectly call ReadDiskSector function to
 * perform actual disk I/O. Function can be called when removable device is not
 * present, error code ERROR_NOT_READY is returned.
 * @param sector Sector number.
 * @param buffer Data buffer where to put the data from the sector.
 * @return int Error code or 0 if no errors.
 */
int BlockDevice::ReadSector(int sector, void * buffer)
{
	if (!detected)
		return ENODEV;
	Message message;
	memset(&message, 0, sizeof(Message));
	message.code = Read;
	message.sector = sector;
	message.buffer = (uint8 *) buffer;
	Fifo<int> response(1, "Read");
	message.result = &response;
	queue.Write(0, message, -1);
	int result;
	response.Read(0, result, -1);
	return result;
}

/**
 * WriteSector: Write a sector to the block device. This is the function to call
 * when a sector needs to be written. The function is executed from caller task
 * context. Function passes messages to the block device task to perform cache
 * management, and then it may indirectly call WriteDiskSector function to
 * perform actual disk I/O. Function can be called when removable device is not
 * present, error code ERROR_NOT_READY is returned.
 * @param sector Sector number.
 * @param buffer Data buffer where to get the data to write to the sector.
 * @return int Error code or 0 if no errors.
 */
int BlockDevice::WriteSector(int sector, void * buffer)
{
	if (!detected)
		return ENODEV;
	Message message;
	memset(&message, 0, sizeof(Message));
	message.code = Write;
	message.sector = sector;
	message.buffer = (uint8 *) buffer;
	Fifo<int> response(1, "Write");
	message.result = &response;
	queue.Write(0, message, -1);
	int result;
	response.Read(0, result, -1);
	return result;
}

/**
 * ProbeDetectMedia: Probe and detect media for the drive. Function is called
 * in block device context from Run() function. Function uses another function
 * DetectMedia() to perform the detection. ProbeDetectMedia function only
 * manages lost and found actions.
 */
void BlockDevice::ProbeDetectMedia()
{
	bool newDetect = DetectMedia();
	if (newDetect && !detected)
	{
		// Use 100K buffers for the device. This would allocate 3 full clusters
		// for FAT systems or even more usable space on other file systems. The
		// overhead of 200 blocks is still a whole lot less than the overhead
		// of reading extra sectors between long file transfers...
		int blocks = 200;
		int size = GetBlockSize();
		// If device block size is multiple sectors, we can simplify life a bit.
		// Block count goes down, block size goes up, average cache size stays
		// same.
		blocks /= size;
		// For now we are not trying to handle multiple partitions having their
		// own caching.
		if (cacheEnabled)
			InitCache(blocks, size);
		fsManager->NotifyMediaDetected(this);
	}
	if (!newDetect && detected)
	{
		DeleteCache();
		fsManager->NotifyMediaLost(this);
	}
	detected = newDetect;
}

/**
 * ReadSector: Read a sector from cache or drive. This function executes in
 * block device context.
 * @param message Sector read parameter structure. Structure contains sector
 * number, buffer address and the response queue for completion and error
 * return values.
 */
void BlockDevice::ReadSector(Message & message)
{
	int error;
	if (cacheEnabled)
	{
		int sector = message.sector;
		int blockIndex = FindCacheBlockContainingSector(sector);
		CacheBlock * block = cache[blockIndex];
		// If the block is fresh, we need to initialize it. Sector must be the first
		// sector in the block, not the one we wanted to read.
		if (block->IsUnused())
		{
			//printf("\n -block->IsUnused()-\n");
			block->Initialize(sector & ~(cacheBlockSize - 1));
		}
		error = ReadSectorIntoCache(blockIndex, sector);
		int offset = block->GetSectorOffset(sector);
		//printf("\n -block-copiedread sector-\n");
		memcpy(message.buffer, block->GetSectorData(offset), GetSectorSize());
	}
	else
		error = ReadDiskSector(message.sector, message.buffer);
	message.result->Write(this, error, -1);
}

/**
 * WriteSector: Write a sector to the cache, potentially flushing another cache
 * block to the drive.. This function executes in block device context.
 * @param message Sector write parameter structure. Structure contains sector
 * number, buffer address and the response queue for completion and error
 * return values.
 */
void BlockDevice::WriteSector(Message & message)
{
	int result;
	if (cacheEnabled)
	{
		int sector = message.sector;
		int blockIndex = FindCacheBlockContainingSector(sector);
		CacheBlock * block = cache[blockIndex];
		// If the block is fresh, we need to initialize it. Sector must be the first
		// sector in the block, not the one we wanted to read.
		if (block->IsUnused())
		{
			block->Initialize(sector & ~(cacheBlockSize - 1));
		}
		WriteSectorIntoCache(blockIndex, sector, message.buffer);
		// Can't have errors at this point. All we do is copy data from one
		// buffer in memory into another. However, we can't communicate flushing
		// errors to application code later because the application code calls
		// close() and only then we may have any errors.
		result = ERROR_SUCCESS;
	}
	else
	{
		result = WriteDiskSector(message.sector, message.buffer);
	}
	message.result->Write(this, result, -1);
}

/**
 * FindCacheBlockContainingSector: Find a cache block that contains specified
 * sector. Cache is scanned in following order:
 * <ol>
 * <li>All blocks are scanned to find a block containing this sector. If found,
 * index is returned.
 * <li>If previous step did not return the block index, the sector is not
 * present in the cache. Next pass is to look through the second half of blocks
 * trying to find an unused block or the block that has only read data in it.
 * If block has only read data, block has no overhead caused by writing data to
 * the block device. Block is marked as unused (sector -1) and returned.
 * <li>If all passes above failed, we have second half of blocks full of data
 * that needs to be written to the block device. In this case we take the LRU
 * block, flush it, mark it as unused and return it.
 * </ol>
 * @param sector Sector number.
 * @return int Cache block index.
 */
int BlockDevice::FindCacheBlockContainingSector(int sector)
{
	ASSERT(cacheEnabled);
	CacheBlock * block;
	int i;
	bool found = false;
	for (i = 0; i < cacheBlockCount; i++)
	{
		block = cache[i];
		if (block->IsUnused())
		{
			// Unused, past this spot we can't have any used blocks because all
			// used blocks would be more recent than the first unused one.
			break;
		}
		if (block->ContainsSector(sector))
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		// Hmpf... Not in cache. Try to make some room and load the sector...
		// First, try to find a block that does not need to be written back, so
		// we don't need to flush it. Look at second half of the list so we
		// won't have to go through these hoops again when we need to read few
		// more bytes from the sector we just trashed... Also, we need to look
		// backwards so we will get the LRU block from the end of the table,
		// not the half-used block from the middle.
		for (i = cacheBlockCount - 1; i > cacheBlockCount / 2; i--)
		{
			block = cache[i];
			if (block->IsUnused() || block->IsClean())
			{
				// mark the block as 'unused' so we can initialize it later
				block->MarkUnused();
				found = true;
				break;
			}
		}
	}
	if (!found)
	{
		// Still nothing - get the last recently used block, flush it, and use
		// it for reading. At this point it is guaranteed that LRU block is
		// full of data waiting to be written, otherwise we would find the
		// block in earlier passes.
		i = cacheBlockCount - 1;
		block = cache[i];
		FlushCacheBlock(block);
		// Again, mark it to be initialized...
		block->MarkUnused();
	}
	return i;
}

/**
 * FlushCacheBuffer: Write cache buffer to the block device. The assumption
 * is that if block device needs some sector to be erased first, writing into
 * the first sector of the block will do it automatically. This function does
 * not rearrange MRU list of cache blocks.
 * @param index Cache block index.
 */
int BlockDevice::FlushCacheBlock(CacheBlock * block)
{
	ASSERT(cacheEnabled);
	int error = ERROR_SUCCESS;
	// We should have something written into the cache block to get here...
	if (block->IsClean() || block->IsUnused())
	{
		return error;
	}
	if (cacheBlockSize > 1)
	{
		// Block is more than one sector, so we need to read whatever is on the
		// device, so that when the whole block is erased, we can still write
		// back everything in sectors that we didn't modify.
		// TODO This is lots of cache management and very little block device
		// I/O, so maybe this should be a part of cache code...
		unsigned long long mask = 1;
		for (int i = 0; i < cacheBlockSize; i++, mask <<= 1)
		{
			if (block->IsWritten(i))
			{
				// sector is modified, no need to refresh it
				continue;
			}
			if (block->IsRead(i))
			{
				// sector is already read
				continue;
			}
			int err = ReadDiskSector(block->GetSectorByOffset(i),
					block->GetSectorData(i));
			block->MarkRead(i);
			if (err)
				error = err;
		}
	}
	// Now write however many sectors are in the block.

	for (int i = 0; i < cacheBlockSize; i++)
	{
		//		int err = WriteDiskSector(block->GetStartSector() + i,
		//				block->GetSectorData(block->GetStartSector() + i));
		int err = WriteDiskSector(block->GetSectorByOffset(i),
				block->GetSectorData(i));
		if (err)
			error = err;
	}

	block->MarkReadAndClean();
	return error;
}

/**
 * ReadSectorIntoCache: Read a sector from disk into cache block. Cache block
 * is made 'most recently used'.
 * @param index Cache block index where to read the data.
 * @param sector Sector number which to read from the block device. Offset in
 * the cache buffer is determined based on sector count per block.
 * @return int Error code for actual disk read, or ERROR_SUCCESS (0) if no
 * errors.
 */
int BlockDevice::ReadSectorIntoCache(int index, int sector)
{
	ASSERT(cacheEnabled);
	// TODO Move most of this code to CacheBlock class?
	CacheBlock * block = cache[index];
	// Sector is somewhere in this block...
	int offset = block->GetSectorOffset(sector);
	//unsigned long long mask = 1ULL << offset;
	int error = ERROR_SUCCESS;
	if (!block->IsRead(offset) && !block->IsWritten(offset))
	{
		// This part of the block is neither read or written, so we
		// have to read the sector to get the current data.
		error = ReadDiskSector(sector, block->GetSectorData(offset));
		block->MarkRead(offset);
		// Otherwise, the cache buffer would already have either data we read
		// from the block device, or the data written into the sector, and we
		// don't need to read data and we don't have to mess around with
		// read/write bits either.
	}
	MakeMRU(index);
	return error;
}

/**
 * MakeMRU: Make specified cache block the most recently used.
 * @param blockIndex Block index to make the most recent. Everything else is
 * shifted one level down.
 */
void BlockDevice::MakeMRU(int blockIndex)
{
	ASSERT(cacheEnabled);
	if (blockIndex == 0) // already MRU
		return;
	CriticalSection s;
	CacheBlock * block = cache[blockIndex];
	for (int i = blockIndex; i >= 1; i--)
		cache[i] = cache[i - 1];
	cache[0] = block;
}

/**
 * WriteSectorIntoCache: Write sector data into cache block. Cache block is
 * marked MRU.
 * @param index Cache block index where to write data.
 * @param sector Sector number to write.
 * @param buffer Sector data to write.
 */
void BlockDevice::WriteSectorIntoCache(int index, int sector, uint8 * buffer)
{
	ASSERT(cacheEnabled);
	// TODO Move most of this code to CacheBlock class?
	CacheBlock * block = cache[index];
	// Sector is somewhere in this block...
	int offset = block->GetSectorOffset(sector);
	block->MarkWritten(offset);
	block->ResetTimer(cacheTimeout);
	MakeMRU(index);
	memcpy(block->GetSectorData(offset), buffer, GetSectorSize());
}

/**
 * FlushCacheBlock: Flush a single cache block. Block pointer is provided in
 * the message. This function does not rearrange MRU list of cache blocks.
 * @param message Message containing cache block pointer to flush.
 */
void BlockDevice::FlushCacheBlock(Message & message)
{
	ASSERT(cacheEnabled);
	CacheBlock * block = message.block;
	int error = FlushCacheBlock(block);
	// TODO If result is 0, message came from timer tick, but we still need to
	// let something know we got problems
	if (message.result)
		message.result->Write(this, error, -1);
	else if (error != ERROR_SUCCESS)
		// TODO Instead of printing, we should let someone know this failed.
		printf("Background flush failed, error %d\n", error);
}

/**
 * FlushCache: Flush entire cache for this block device. This function does not
 * rearrange MRU list of cache blocks.
 * @return int Error code or 0 if no errors.
 */
int BlockDevice::FlushCache()
{
	ASSERT(cacheEnabled);
	Message message;
	memset(&message, 0, sizeof(Message));
	message.code = FlushAll;
	Fifo<int> response(1, "FlushAll");
	message.result = &response;
	queue.Write(0, message, -1);
	int result;
	response.Read(0, result, -1);
	return result;
}

/**
 * FlushCache: Flush all cache blocks. This function does not rearrange MRU
 * list of cache blocks.
 * @param message Message containing FlushAll command code.
 */
void BlockDevice::FlushCache(Message & message)
{
	ASSERT(cacheEnabled);
	int error = ERROR_SUCCESS;
	for (int blockIndex = 0; blockIndex < cacheBlockCount; blockIndex++)
	{
		int err = FlushCacheBlock(cache[blockIndex]);
		if (err)
			error = err;
	}
	message.result->Write(this, error, -1);
}
