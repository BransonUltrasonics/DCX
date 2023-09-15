// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/BlockDevices/CacheBlock.h_v   1.0   09 Jun 2015 09:13:36   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares disk cache block object.

#ifndef CACHEBLOCK_H_
#define CACHEBLOCK_H_

/**
 * CacheBlock: Structure is used for data caching on the block device. Each
 * block contains a data buffer for all sectors belonging to this block. It
 * is up to the derived task to decide how many sectors must belong to one
 * cache block, depending on drive properties. SD card and disk devices use
 * one sector per buffer, but NAND flash memory, for example, is better off
 * using memory block sized cache buffers because NAND flash supports erase
 * only for the whole block and it is more efficient if sectors belonging
 * to same flash memory block are also combined into single cache block.
 */
class CacheBlock
{
	/**
	 * enum Parameters: Common caching parameters.
	 */
	enum Parameters
	{
		/**
		 * UnusedSector: Value for sector number for the unused cache block.
		 */
		UnusedSector = -1,
	};
public:
	CacheBlock(int cacheBlockSize, int cacheSectorSize);
	virtual ~CacheBlock();
	void Initialize(int startSector = UnusedSector);
	/**
	 * IsDirty: Returns true if cache block has data that is not yet written to
	 * the drive.
	 * @return bool true if some sectors must be written to the drive.
	 */
	bool IsDirty() const
	{
		return write != 0;
	}
	/**
	 * IsClean: Return true if cache block is clean. Clean cache block has no
	 * sectors that must be written to the block device.
	 * @return bool true if all data is current.
	 */
	bool IsClean() const
	{
		return !IsDirty();
	}
	/**
	 * ResetTimer: Initialize cache block timer to the specified value. Cache
	 * block timer is initially set to the timeout value and then is being
	 * decremented by specified amount in every pass of timer tick if the block
	 * contains written but not flushed data. When timer expires, data is
	 * written to the block device.
	 * @param timerUsec: Initial value of the cache timer in microseconds.
	 */
	void ResetTimer(int timerUsec)
	{
		timerUs = timerUsec;
	}
	/**
	 * UpdateTimer: Update cache block timer by specified microseconds. Cache
	 * block timer is initially set to the timeout value and then is being
	 * decremented by specified amount in every pass of timer tick if the block
	 * contains written but not flushed data. When timer expires, data is
	 * written to the block device.
	 * @param timeUsec Timer update value in microseconds.
	 */
	void UpdateTimer(int timeUsec)
	{
		timerUs -= timeUsec;
	}
	/**
	 * IsTimerExpired: Return true if timer is expired. Cache block timer is
	 * initially set to the timeout value and then is being decremented by
	 * specified amount in every pass of timer tick if the block contains
	 * written but not flushed data. When timer expires, data is written to
	 * the block device.
	 * @return bool True if cache block timer expired and data must be written
	 * to the block device.
	 */
	bool IsTimerExpired() const
	{
		return timerUs <= 0;
	}
	/**
	 * IsUnused: Return true if the cache block is not used yet. Cache blocks
	 * initially have firstSector value UnusedSector.
	 * @return bool true if cache block is unused.
	 */
	bool IsUnused() const
	{
		return firstSector < 0;
	}
	/**
	 * GetSectorData: Return pointer to the specified sector data in the block
	 * data buffer.
	 * @param offset Zero-based sector offset in the cache block.
	 * @return unsigned char * Sector data in the block data buffer.
	 */
	unsigned char * GetSectorData(int offset) const
	{
		return data + offset * sectorSize;
	}
	/**
	 * ContainsSector: Return true if the specified sector is cached by this
	 * cache block.
	 * @param sector Absolute sector number to verify.
	 * @return bool true if sector is within this cache block.
	 */
	bool ContainsSector(int sector) const
	{
		return (sector >= firstSector) && (sector < (firstSector + blockSize));
	}
	/**
	 * MarkUnused: Mark cache block as unused.
	 */
	void MarkUnused()
	{
		firstSector = UnusedSector;
	}
	/**
	 * IsWritten: Return true if sector in the cache block is written but not
	 * yet stored on the block device.
	 * @param offset Relative sector number in the cache block.
	 * @return bool True if sector needs to be written to the block device.
	 */
	bool IsWritten(int offset) const
	{
		unsigned long long mask = 1ULL << offset;
		return write & mask;
	}
	/**
	 * IsRead: Return true if sector in the cache block is read from the block
	 * device. Read sectors do not need extra synchronization with the block
	 * device, assuming there is no direct access to the device that bypasses
	 * the disk cache. All sectors are 'unread' when the cache block is first
	 * initialized.
	 * @param offset Relative sector number in the cache block.
	 * @return bool True if sector is already read from the block device.
	 */
	bool IsRead(int offset) const
	{
		unsigned long long mask = 1ULL << offset;
		return read & mask;
	}
	/**
	 * MarkWritten: Mark a sector 'written but not updated'. Written sectors
	 * contain the most recent data in the cache block, but block device is not
	 * yet updated.
	 * @param offset Relative sector number in the cache block.
	 */
	void MarkWritten(int offset)
	{
		unsigned long long mask = 1ULL << offset;
		write |= mask;
	}
	/**
	 * MarkRead: Mark a sector 'read from the block device'. Read sectors
	 * contain the most recent data from the block device.
	 * @param offset Relative sector number in the cache block.
	 */
	void MarkRead(int offset)
	{
		unsigned long long mask = 1ULL << offset;
		read |= mask;
	}
	/**
	 * MarkReadAndClean: Mark all written sectors as read and clean. This
	 * function is called when cache block is written to the block device.
	 * After this function call all sectors contain the most recent data
	 * synchronized with the block device ('read' = 1) and none of it needs to
	 * be written back to the block device ('written' = 0).
	 */
	void MarkReadAndClean()
	{
		read |= write;
		write = 0;
	}
	/**
	 * GetSectorOffset: Get the sector offset in the cache block from the
	 * absolute sector number.
	 * @param sector Sector absolute number.
	 * @return int Offset in range 0...blockSize - 1.
	 */
	int GetSectorOffset(int sector) const
	{
		return sector - firstSector;
	}
	/**
	 * GetSectorByOffset: Return absolute sector number from the sector offset.
	 * @param offset Sector offset in the cache block.
	 * @return Sector absolute number.
	 */
	int GetSectorByOffset(int offset) const
	{
		return firstSector + offset;
	}
protected:
	/**
	 * blockSize: Cache block size in sectors. Each write to the block device
	 * contains this many sectors. If block device needs erasing before writing
	 * data, this cache block size must match the block device block size, so
	 * the block device driver can perform erase before writing data to the
	 * device. This block erase is only performed for block sizes more than
	 * one sectors.
	 */
	int blockSize;
	/**
	 * sectorSize: Cache sector size in bytes.
	 */
	int sectorSize;
	/**
	 * firstSector: First sector number cached in this block. Sector number is a
	 * multiple of cache block size.
	 */
	int firstSector;
	/**
	 * data: Data buffer for all sectors in the cache for this block.
	 */
	unsigned char * data;
	/**
	 * read: Bit mask for each sector in the buffer whether it has been
	 * read from the device.
	 * TODO: This implementation assumes that cache block is maximum 64
	 * sectors in size.
	 */
	unsigned long long read;
	/**
	 * write: Bit mask for each sector in the buffer whether it has been
	 * written into and needs to be stored on the device.
	 * TODO: This implementation assumes that cache block is maximum 64
	 * sectors in size.
	 */
	unsigned long long write;
	/**
	 * timerUs: Buffer timer. Timer is set to maximum value at each write
	 * and it counts down in timer tick. When timer reaches zero, buffer is
	 * written to the block device.<p>
	 * <b>This design would also cause buffer to never be written if some
	 * task goes crazy and keeps overwriting the sector. This will lose the
	 * data, but will not wear out the drive if it is a flash memory of
	 * some kind.</b>
	 */
	int timerUs;
};

#endif
