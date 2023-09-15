/*
 * FRAMDiskDevice.h
 *
 *  Created on: Mar 22, 2012
 *      Author: rjamloki
 */

#ifndef FRAMDISKDEVICE_H_
#define FRAMDISKDEVICE_H_

#include "BlockDevice.h"

class FRAMDiskDevice: public BlockDevice
{
public:
	FRAMDiskDevice(char * name);
	virtual ~FRAMDiskDevice();
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

	virtual int GetSectorSize() const
	{
			return 2048;
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
	unsigned char * fData;
	bool mediaPresent;
};

#endif /* FRAMDISKDEVICE_H_ */
