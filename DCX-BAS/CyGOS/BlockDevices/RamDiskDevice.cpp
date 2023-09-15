// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/BlockDevices/RamDiskDevice.cpp_v   1.0   21 Mar 2012 12:34:58   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares generic RAMDisk device driver.

// FIXME RamDisk init/remove
// After drive is initialized and removed, during next init FileSystemManager
// reports 'all valid drive letters are busy'. Drive is not removed from active
//letter assignments.

#include "RamDiskDevice.h"
#include "errno.h"
#include "stdio.h"
#include "string.h"
#include "FileSystemManager.h"
#include "DebugNew.h"

BlockDevice * CreateRamDisk()
{
	return new RamDiskDevice("RamDisk");
}

RamDiskDevice::RamDiskDevice(char * name) :
	BlockDevice(name, 2048, false)
{
	buf = 0;
	mediaPresent = false;
	detectTimeout = 500000;
}

RamDiskDevice::~RamDiskDevice()
{
	// TODO Check if file system manager already forgot about this drive...
	if (buf)
		delete buf;
}

int RamDiskDevice::Initialize(int size, const void * data)
{
	if (mediaPresent)
		return EBUSY;
	buf = new unsigned char[size + GetSectorSize()];//Extra space for MBR
	memset(buf, 0, GetSectorSize());
	sectorCount = (size / GetSectorSize());
	MBR * mbr = (MBR *) buf;
	memset(mbr, 0x00, sizeof(MBR));
	PutWord(mbr->hex55AA, 0xAA55);
	PutDWord(mbr->partition[0].lba, 1);
	PutDWord(mbr->partition[0].length, sectorCount);
	mbr->partition[0].type = 0xC6;
	memcpy(buf + GetSectorSize(), data, size);
	mediaPresent = true;
	return ERROR_SUCCESS;
}

// TODO Issue 1054, do we delete data in here or elsewhere?
int RamDiskDevice::Remove()
{
	mediaPresent = false;
	delete buf;
	return ERROR_SUCCESS;
}

/**
 * DetectMedia: Try to detect the removable media that may be present in the
 * drive.
 * @return bool True if media is in the drive.
 */
bool RamDiskDevice::DetectMedia()
{
	return mediaPresent;
}

/**
 * GetSectorCount:
 *
 * @return int
 */
int RamDiskDevice::GetSectorCount() const
{
	return sectorCount;
}

/**
 * ReadDiskSector: Read the physical sector on the drive.
 * @param sector Sector number to read.
 * @param data Data buffer where to put the sector data.
 * @return int Error code or 0 if no errors. Possible errors are:
 * <ul>
 * <li>ERROR_READ_FAULT: SD card interface error - no response within 50 bytes.
 * <li>ERROR_CRC: CRC error transferring data from SD card.
 * <li>ERROR_GEN_FAILURE: Response to READ_SINGLE_BLOCK command was not 0.
 * <li>ERROR_SECTOR_NOT_FOUND: The drive cannot find the sector requested.
 * </ul>
 */
int RamDiskDevice::ReadDiskSector(int sector, unsigned char* data)
{
	if (buf == 0)
		return EBUSY;
	if ((sector < 0) && (sector >= sectorCount))
		return EFAULT;
	unsigned char * tempbuf = buf + sector * GetSectorSize();
	if (tempbuf > (buf + sectorCount * GetSectorSize()))
		return EFAULT;
	memcpy(data, tempbuf, GetSectorSize());
	return ERROR_SUCCESS;
}

/**
 * WriteDiskSector: Write the physical sector to the drive.
 * @param sector Sector number to write.
 * @param data Data buffer where to get the sector data.
 * @return int Error code or 0 if no errors. Possible errors are:
 * <ul>
 * <li>ERROR_SECTOR_NOT_FOUND: The drive cannot find the sector requested.
 * <li>ERROR_GEN_FAILURE: Response to WRITE_BLOCK command was not 0.
 * <li>ERROR_CRC: CRC error transferring data from SD card.
 * <li>ERROR_WRITE_FAULT: SD card reported write error in response to sector
 * data, or SD card reported busy writing for more than 50ms.
 * </ul>
 */
int RamDiskDevice::WriteDiskSector(int sector, unsigned char* data)
{
	if (buf == 0)
		return EBUSY;
	if ((sector < 0) && (sector >= sectorCount))
		return EFAULT;
	unsigned char * tempbuf = buf + sector * GetSectorSize();
	if (tempbuf > (buf + sectorCount * GetSectorSize()))
		return EFAULT;
	memcpy(tempbuf, data, GetSectorSize());
	return ERROR_SUCCESS;
}
