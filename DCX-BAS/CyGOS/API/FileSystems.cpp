// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/API/FileSystems.cpp_v   1.5   08 Oct 2013 06:12:06   rjamloki  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares CyGOS functions to initialize block devices and file
// systems.

#include "CyGOS.h"
#include "FileSystemManager.h"
#include "BlockDevice.h"
#include "RTOS.h"
#include "Decode.h"

FileSystemManager * fsManager;

void CyGOS::InitFileSystems(int & priority)
{
	fsManager = new FileSystemManager();
	RTOS::Create(fsManager, priority++);
}

void CyGOS::CreateBlockDevice(BlockDevice * device, const char * letters,
		int & priority)
{
	RTOS::Create(device, priority++);
	fsManager->RegisterDrive(device, letters);
}

int CyGOS::ReadSector(int drive, int sector, int count, void * buffer)
{
	return fsManager->ReadSector(drive, sector, count, buffer);
}

int CyGOS::WriteSector(int drive, int sector, int count, void * buffer)
{
	return fsManager->WriteSector(drive, sector, count, buffer);
}

int CyGOS::InitDriveImage(int drive, void * image, int length)
{
	return fsManager->InitDriveImage(drive, image, length);
}

int CyGOS::RemoveDrive(int drive)
{
	return fsManager->RemoveDrive(drive);
}

int CyGOS::FileOpen(const char * fileName, int mode, File ** f)
{
	return fsManager->FileOpen(fileName, mode, f);
}

void CyGOS::FileClose(void * file)
{
	fsManager->FileClose(file);
}

int CyGOS::FileRead(void * file, void * buffer, int * length)
{
	return fsManager->FileRead(file, buffer,length);
}

int CyGOS::FileLength(void * file)
{
	return fsManager->FileLength(file);
}


/* 	void ReadTarRegionFlash(int drive, unsigned char * tarStartAddress, lenghtImage)
 *
 *   Purpose:
 *		This function is called once at power up from Main() function.It reads the TAR image from flash
 *		and allocates a buffer in RAM to copy the image. It also relates image to drive.
 *
 *   Entry condition:
 *		Drive - The drive number. For drive ‘A’ its 0. For drive ‘B’ and further its (driveLetter – ‘A’). e.g. for drive ‘B’ its 1(‘B’ – ‘A’)
 *		tarStartAddress - The address in flash where compressed tar image is programmed
 *		lengthImage- Max length of TAR image.
 *
 *   Exit condition:
 *		None
 */
unsigned int ReadTarRegionFlash(int drive, unsigned char * tarStartAddress, int lengthImage)
{
	//	Find the actual Tar image size from initial 32 bit in image.
	unsigned int resultLength = GetDWord(tarStartAddress);
	pprintf("\n tar area length is %d", resultLength);
	if((resultLength != (unsigned int)0xFFFFFFFF) && (resultLength != 0x00) && (resultLength <= ((unsigned int)lengthImage - 4)))
	{
		unsigned char * TarImage = new unsigned char[resultLength];
		memcpy((void *) TarImage, (unsigned long *)(tarStartAddress + 4), resultLength);
		pprintf("\n ctarIm %08X tarstrt %08X length = %d isProgrammed %d",
					(void *) TarImage, (unsigned long *)tarStartAddress, lengthImage, resultLength);
//		pprintf("\n Compressed Data\n");
//		for(int i = 0; i < lengthImage; i++){
//
//			pprintf("%02X ", compressedTarImage[i]);
//		}
		CyGOS::InitDriveImage(drive, TarImage, resultLength);		
		delete TarImage;
		resultLength = resultLength + 4;//include length in CRC
	}
	else
		resultLength = 0;

	return resultLength;
}

