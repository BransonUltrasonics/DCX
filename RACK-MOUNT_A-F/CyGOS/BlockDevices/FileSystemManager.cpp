// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/BlockDevices/FileSystemManager.cpp_v   1.1   24 Jul 2015 08:56:04   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares file system manager object.

#include "FileSystemManager.h"
#include "BlockDevice.h"
#include "FileSystem.h"
#include "RTOS.h"
#include "stdio.h"
#include "assert.h"
#include "errno.h"
#include "trace.h"
#include "DebugNew.h"

FileSystemManager::FileSystemManager() :
	Task("FSManager", 1024), queue(10, "FSQ")
{
	memset(logicalDrive, 0, sizeof(logicalDrive));
	memset(physicalDevices, 0, sizeof(physicalDevices));
	printf("FSM created at %p\n", this);
}

FileSystemManager::~FileSystemManager()
{
}

void FileSystemManager::Run()
{
	for (;;)
	{
		FSMessage message;
		queue.Read(this, message, -1);

		switch (message.command)
		{
		case fsDriveDetected:
			MediaDetected(message);
			break;
		case fsDriveLost:
			MediaLost(message);
			break;
		default:
			CRASH("FSQ: Bad message");
		}

	}
}

void FileSystemManager::NotifyMediaDetected(BlockDevice * device)
{
	printf("Notify: Media detected, drive %s\n", device->GetName());
	FSMessage message;
	printf(".");
	message.command = fsDriveDetected;
	printf(".");
	message.device = device;
	queue.Write(0, message, -1);
}

void FileSystemManager::NotifyMediaLost(BlockDevice * device)
{
	printf("Notify: Media lost, drive %s\n", device->GetName());
	FSMessage message;
	printf(".");
	message.command = fsDriveLost;
	printf(".");
	message.device = device;
	queue.Write(0, message, -1);
}

void FileSystemManager::MediaDetected(FSMessage & message)
{
	printf("Manager: Drive %s detected\n", message.device->GetName());
	int drive = -1;
	POSITION pos = drives.GetHeadPosition();
	while (pos)
	{
		DriveEntry * entry = drives.GetNext(pos);
		if (entry->device == message.device)
		{
			pprintf("Found device %s\n", entry->device->GetName());
			for (int i = 0; entry->letters[i]; i++)
			{
				int d = entry->letters[i] - 'A';
				if (physicalDevices[d] == 0)
				{
					drive = d;
					break;
				}
			}
			break;
		}
	}
	if (drive == -1)
	{
		printf("All valid drive letters are busy\n");
		return;
	}
	printf("Drive number %d is handled by %s\n", drive,
			message.device->GetName());
	physicalDevices[drive] = message.device;
	TRACE_8X((unsigned int)physicalDevices[drive]);
	if (message.device->IsPartitionedDevice())
	{
		printf("Drive is partitioned\n");
		DelayMs(100);
		uint8 * sector = new uint8[message.device->GetSectorSize()];
		int error = message.device->ReadSector(0, sector);
		if (error)
			printf("Error reading MBR\n");
		else
		{
			MBR * mbr = (MBR *) sector;
			printf("Master Boot Record:\n");
			printf("Signature %02X %02X\n", mbr->hex55AA[0], mbr->hex55AA[1]);
			int lba = mbr->partition[0].GetStartLBA();
			printf("LBA-first %d\n", lba);
			int length = mbr->partition[0].GetLength();
			printf("LBA-length %d\n", length);
			printf("MBR signature verified\n");
			int type = mbr->partition[0].type;
			printf("Partition type: %02X\n", type);
			POSITION pos1 = fileSystems.GetHeadPosition();
			while (pos1)
			{
				FileSystemEntry * fs = fileSystems.GetNext(pos1);
				if (fs->type == type)
				{
					FileSystem * fsTask = fs->create(message.device, type, lba,
							length);
					printf("New %s task at %p, ", fsTask->GetName(), fsTask);
					int prio = RTOS::Create(fsTask, fs->prioLow, fs->prioHigh);
					printf("Priority %d\n", prio);
					logicalDrive[drive] = fsTask;
					break;
				}
			}
		}
		delete sector;
	}
	else
	{
		printf("Drive is not partitioned\n");
		int sectors = message.device->GetSectorCount();
		if (sectors == -1)
		{
			printf("Unknown non-partitioned device size!\n");
		}
		else
		{
			printf("Got fixed device size of %d sectors\n", sectors);
		}
	}
	//		DelayMs(100);
}

void FileSystemManager::MediaLost(FSMessage & message)
{
	printf("Manager: Drive %s lost\n", message.device->GetName());
	// First check - for all file systems on drive that was lost, clean it up
	for (int i = 0; i < 26; i++)
	{
		FileSystem * fsTask = logicalDrive[i];
		if (fsTask->device == message.device)
		{
			logicalDrive[i] = 0;
			physicalDevices[i] = 0;
			RTOS::Delete(fsTask);
			delete fsTask;
		}
	}
	// Second check - if there were no file systems activated on the drive, we
	// still need to release the physical device table entry
	for (int i = 0; i < 26; i++)
	{
		if (physicalDevices[i] == message.device)
			physicalDevices[i] = 0;
	}
}

void FileSystemManager::RegisterDrive(BlockDevice * device,
		const char * letters)
{
	DriveEntry * entry = new DriveEntry();
	entry->device = device;
	entry->letters = letters;
	drives.AddTail(entry);
}

void FileSystemManager::RegisterFS(int type, const char * name,
		CreateFS createFSTask, int prioLow, int prioHigh)
{
	FileSystemEntry * entry = new FileSystemEntry();
	entry->type = type;
	entry->name = name;
	entry->create = createFSTask;
	entry->prioLow = prioLow;
	entry->prioHigh = prioHigh;
	fileSystems.AddTail(entry);
	TRACE_S(name);
	TRACE_D(type);
	TRACE_D(fileSystems.GetCount());
}

// TODO Drive addressing needs some work!
BlockDevice * FileSystemManager::FindDrive(int drive)
{
	POSITION pos = drives.GetHeadPosition();
	while (pos)
	{
		DriveEntry * entry = drives.GetNext(pos);
		if (strchr(entry->letters, drive + 'A') != 0)
		{
			return entry->device;
		}
	}
	return 0;
}

int FileSystemManager::ReadSector(int drive, int sector, int count,
		void * buffer)
{
	BlockDevice * device = FindDrive(drive);
	if (device)
	{
		printf("Found device '%s'\n", device->GetName());
		int error = ERROR_SUCCESS;
		uint8 * data = (uint8 *) buffer;
		for (int i = 0; i < count; i++)
		{
			error = device->ReadSector(sector, data);
			if (error != ERROR_SUCCESS)
				break;
			sector++;
			data += device->GetSectorSize();
		}
		return error;
	}
	return EFAULT;
}

int FileSystemManager::WriteSector(int drive, int sector, int count,
		void * buffer)
{
	BlockDevice * device = FindDrive(drive);
	if (device)
	{
		printf("Found device '%s'\n", device->GetName());
		int error = ERROR_SUCCESS;
		uint8 * data = (uint8 *) buffer;
		for (int i = 0; i < count; i++)
		{
			error = device->WriteSector(sector, data);
			if (error != ERROR_SUCCESS)
				break;
			sector++;
			data += device->GetSectorSize();
		}
		return error;
	}
	return EFAULT;
}

int FileSystemManager::InitDriveImage(int drive, void * image, int length)
{
	BlockDevice * device = FindDrive(drive);
	if (device)
	{
		printf("Found device '%s'\n", device->GetName());
		return device->Initialize(length, image);
	}
	return EFAULT;
}

int FileSystemManager::RemoveDrive(int drive)
{
	BlockDevice * device = FindDrive(drive);
	if (device)
	{
		printf("Found device '%s'\n", device->GetName());
		return device->Remove();
	}
	return EFAULT;
}

/**
 * FindDriveNumber: Find drive number from the file name.
 * @param fileName Full file name, including drive letter.
 * @return int Drive number (0 - A, 1 - B etc) or -1 if file name does not start
 * with &lt;drive letter&gt;&lt;:&gt;&lt;&gt;.
 */
int FileSystemManager::FindDriveNumber(const char * path)
{
	if (path[1] != ':')
		return -1;
	if ((path[2] != '\\')/*&&(path[2] != '/')*/)
		return -1;
	int driveNumber = -1;
	if ((path[0] >= 'a') && (path[0] <= 'z'))
		driveNumber = path[0] - 'a';
	if ((path[0] >= 'A') && (path[0] <= 'Z'))
		driveNumber = path[0] - 'A';
	return driveNumber;
}

/**
 * Open the specified file. Function locates the drive that holds the file and
 * opens it. Function executes in user task context.
 * @return int Error code or ERROR_SUCCESS.
 */
int FileSystemManager::FileOpen(const char * path, int oflags, File ** file)
{
	int driveNumber = FindDriveNumber(path);
	if (driveNumber != -1)
	{
		if (logicalDrive[driveNumber])
		{
			return logicalDrive[driveNumber]->FileOpen(path, oflags, file);
		}
	}
	pprintf("\n drive not found");
	return EACCES;
}

int FileSystemManager::Remove(const char * path)
{
	if ((strchr(path, '*') != 0) || (strchr(path, '?') != 0))
		return ENOENT;
	int driveNumber = FindDriveNumber(path);
	if (driveNumber != -1)
	{
		if (logicalDrive[driveNumber])
		{
			return logicalDrive[driveNumber]->Remove(path);
		}
	}
	return EACCES;
}

int FileSystemManager::Unlink(const char * path)
{
	if ((strchr(path, '*') != 0) || (strchr(path, '?') != 0))
		return ENOENT;
	int driveNumber = FindDriveNumber(path);
	if (driveNumber != -1)
	{
		if (logicalDrive[driveNumber])
		{
			return logicalDrive[driveNumber]->Unlink(path);
		}
	}
	return EACCES;
}

int FileSystemManager::FileRead(void * file, void * buffer, int * length)
{
	ASSERT(file);
	File * f = (File*)file;
	int driveNumber = FindDriveNumber(((File*) file)->name);
	if (driveNumber != -1)
	{
		if (logicalDrive[driveNumber])
		{
			return logicalDrive[driveNumber]->FileRead(f, buffer,
					length);
		}
	}

	return -1;

}

int FileSystemManager::FileLength(void * file)
{
	ASSERT(file);
	File * f = (File*)file;
	int driveNumber = FindDriveNumber(f->name);
	if (driveNumber != -1)
	{
		if (logicalDrive[driveNumber])
		{
			return logicalDrive[driveNumber]->FileLength(f);
		}
	}
	return 0;
}

void FileSystemManager::FileClose(void * file)
{
	ASSERT(file);
	File * f = (File*)file;
	int driveNumber = FindDriveNumber(f->name);
	if (driveNumber != -1)
	{
		if (logicalDrive[driveNumber])
		{
			logicalDrive[driveNumber]->FileClose(f);
		}
	}
}


