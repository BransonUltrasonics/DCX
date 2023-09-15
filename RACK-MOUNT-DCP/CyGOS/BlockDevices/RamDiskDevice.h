// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/BlockDevices/RamDiskDevice.h_v   1.0   18 Aug 2014 09:16:16   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares generic RAMDisk device driver.

#ifndef RAMDISK_H_
#define RAMDISK_H_

#include "BlockDevice.h"

class RamDiskDevice: public BlockDevice
{
public:
	RamDiskDevice(char * name);
	virtual ~RamDiskDevice();
	virtual bool DetectMedia();
	int ReadDiskSector(int sector, uint8 * data);
	int WriteDiskSector(int sector, uint8 * data);
	int Initialize(int size, const void * data);
	int Remove();
protected:
	virtual int GetSectorCount() const;
	/**
	 * GetBlockSize:
	 *
	 * @return virtual int
	 */
	virtual int GetBlockSize() const
	{
		return 1;
	}
	/**
	 * IsPartitionedDevice:
	 *
	 * @return virtual bool
	 */
	virtual bool IsPartitionedDevice() const
	{
		return true;
	}
	/**
	 * sectorCount: Sector count on this drive.
	 */
	int sectorCount;

	/** Ram Disk allocated memory in RAM */
	unsigned char *buf;
	bool mediaPresent;
};

#endif
