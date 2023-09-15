// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/BlockDevices/CacheBlock.cpp_v   1.0   09 Jun 2015 09:13:36   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares disk cache block object.

#include "CacheBlock.h"
#include "string.h"
#include "DebugNew.h"

// TODO Maybe implement 3 versions of cache blocks: 1 sector (SD), up to 32
// sectors (small flash), up to 64 sectors (large flash) and unlimited (for
// who knows what...). CacheBlock1, CacheBlock32, CacheBlock64 and CacheBlockX
// would be derived from this class and implement few virtual functions here
// and there.

// TODO Should pull more cache-specific code out of BlockDeviceTask.
// It works fine for now, so it's kinda left half-way there...

/**
 * CacheBlock: Create and initialize the cache block.
 * @param cacheBlockSize Cache block size in sectors.
 * @param cacheSectorSize Cache sector size in bytes.
 */
CacheBlock::CacheBlock(int cacheBlockSize, int cacheSectorSize)
{
	blockSize = cacheBlockSize;
	sectorSize = cacheSectorSize;
	data = new unsigned char[cacheBlockSize * cacheSectorSize];
	Initialize(UnusedSector);
}

/**
 * ~CacheBlock: Destroy the cache block.
 */
CacheBlock::~CacheBlock()
{
	delete data;
}

/**
 * Initialize: Initialize the cache block. All read and write flags are set to
 * zeros - nothing read yet and nothing to be written. Timer is also set to
 * zero. Data area is initialized with all FF. Only first sector is set as
 * specified in the function call. Application code (block device) calls this
 * function when sector is read or written into previously unused cache block.
 * @param startSector First sector number for this cache block. If parameter is
 * not provided, UnusedSector value is used.
 */
void CacheBlock::Initialize(int startSector)
{
	read = 0;
	write = 0;
	firstSector = startSector;
	timerUs = 0;
	memset(data, 0xFF, blockSize * sectorSize);
}
