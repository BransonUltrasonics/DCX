// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/API/FileSystems.cpp_v   1.4   02 Aug 2011 07:16:00   pdwivedi  $
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


void DecompressTarRegionFlash(int drive)
{
	FileDecode *Lzari = new FileDecode();
	int resultLength = 0;
	unsigned char * compressedTarImage = new unsigned char[(int)__TAR_SIZE];
	memcpy((void *) compressedTarImage, (unsigned long *)__TAR_START,(int) __TAR_SIZE);

	unsigned int isProgrammed = GetDWord(compressedTarImage);
	if((isProgrammed != 0xFFFFFFFF) && (isProgrammed != 0x00) && (isProgrammed <= ((unsigned int)__TAR_SIZE)*4))
	{
		byte * result = Lzari->UnCompress(compressedTarImage,
				resultLength);
		CyGOS::InitDriveImage(drive, result, resultLength);
	}
	delete compressedTarImage;
	//TODO:first param is incorrect
	delete Lzari;

}

