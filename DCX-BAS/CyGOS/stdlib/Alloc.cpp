// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/stdlib/Alloc.cpp_v   1.6   25 Oct 2013 11:57:16   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements memory allocation and deallocation. It is taken from
// Freescale sample code and used as per license. The main reason of using
// our own allocation routines is to avoid all the overhead that compiler pulls
// in from runtime libraries.

#include "ArchDefs.h"
#include "Board.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"
#include "stdio.h"
#include "RTOS.h"

#undef DEBUG_NEW
#undef new

#define SAFETY_BEGIN	0x10
#define SAFETY_END 		0x10

static int safetyBegin = SAFETY_BEGIN;
static int safetyEnd = SAFETY_END;
static int safetyTest = true;
static int safetyFill = true;

#define ALLOC_TRACE 0

/**
 * MemHeader: This struct forms the minimum block size which is allocated, and
 * also forms the linked list for the memory space used with alloc() and free().
 */
struct MemHeader
{
	/**
	 * blockSize: Memory block size in bytes. Size is always a multiple of 16
	 * bytes. If safety buffers are used, their sizes are included in this
	 * value.
	 */
	int blockSize;
	/**
	 * allocSize: Allocated size of the memory block. This value is same as the
	 * block size for free blocks, and it contains the exact number of bytes
	 * for allocated blocks. Value may be used for checking writes past the
	 * allocated memory. Safety buffer sizes are not included in this value.
	 */
	int allocSize;
	/**
	 * next: Next memory block in the chain. All free and allocated blocks are
	 * linked together through this pointer.
	 */
	MemHeader * next;
	/**
	 * prev: Previous memory block in the chain. All free and allocated blocks
	 * are linked together through this pointer.
	 */
	MemHeader * prev;
	/**
	 * file: Source file name where the block was allocated. Free blocks have
	 * file name "free", allocated blocks with source not available use file
	 * name "Unknown".
	 */
	char * file;
	/**
	 * line: Source file line number where the block was allocated. If source
	 * file information is not available, line number is 0. Line number -1
	 * is used for free memory blocks. This field is the only one showing
	 * whether the block is allocated or not.
	 */
	int line;
	/**
	 * nextFree: Next free block in the list.
	 */
	MemHeader * nextFree;
	/**
	 * prevFree: Previous free block in the list.
	 */
	MemHeader * prevFree;
	/**
	 * data: Placeholder for data area in the memory block. This area includes
	 * optional safety buffers before and after the allocated amount.
	 */
	unsigned char data[0];
	//	void Trace();
	void PTrace();
	void FillDeallocatedData();
	void FillSafetyAreas(bool init);
	bool SafetyTest(bool quiet = false);
};

void MemHeader::PTrace()
{
	pprintf("%08X: ", this);
	char * p = (char *) this;
	p += sizeof(*this);
	p += safetyBegin;
	pprintf("%08X ", p);
	char * f = strrchr(file, '/');
	if (!f)
		f = file;
	pprintf("%-25s ", f);
	pprintf("%4d ", line);
	pprintf("%9d ", allocSize);
	pprintf("%9d ", blockSize);
	CyGOS::KickWatchDog();
	for (int i = 0; i < 16; i++)
		pprintf("%02X ", data[safetyBegin + i]);
	CyGOS::KickWatchDog();
	if ((line != -1) && (safetyTest))
	{
		bool safetyFailed = false;
		for (int i = 0; i < safetyBegin; i++)
		{
			if (data[i] != 0xBE)
			{
				safetyFailed = true;
				break;
			}
		}
		pprintf(safetyFailed ? "B " : "  ");
		CyGOS::KickWatchDog();
		safetyFailed = false;
		for (int i = 0; i < safetyEnd; i++)
		{
			//if (data[safetyBegin + allocSize + i] != 0xAF)
			if (data[blockSize - safetyEnd + i] != 0xAF)
			{
				safetyFailed = true;
				break;
			}
		}
		// Check the extra gap between 'block' and 'after'
		int offset = safetyBegin + allocSize;
		while ((offset & 0x0000000F) != 0)
		{
			if (data[offset++] != 0xAE)
			{
				safetyFailed = true;
				break;
			}
		}
		pprintf(safetyFailed ? "A" : " ");
	}
	CyGOS::KickWatchDog();
	pprintf("\n");
}

void MemHeader::FillSafetyAreas(bool init)
{
	for (int i = 0; i < safetyBegin; i++)
		data[i] = 0xBE;
	if (init)
	{
		unsigned int * ptr = (unsigned int *) (data + safetyBegin);
		for (int i = 0; i < allocSize; i += sizeof(unsigned int))
			*ptr++ = 0xDEADFACE;
	}
	for (int i = 0; i < safetyEnd; i++)
		//data[safetyBegin + allocSize + i] = 0xAF;
		data[blockSize - safetyEnd + i] = 0xAF;
	// Fill the safety gap between 'block' and 'after'
	int offset = safetyBegin + allocSize;
	while ((offset & 0x0000000F) != 0)
		data[offset++] = 0xAE;
}

void MemHeader::FillDeallocatedData()
{
	unsigned int * ptr = (unsigned int *) &data[safetyBegin];
	*ptr++ = 0x0DEA110C;
	*ptr++ = 0x0DEA110C;
	*ptr++ = 0x0DEA110C;
	*ptr++ = 0x0DEA110C;
}

/**
 * @return true if OK, false if corrupted.
 */
bool MemHeader::SafetyTest(bool quiet)
{
	bool corrupt = false;
	for (int i = 0; i < safetyBegin; i++)
	{
		if (data[i] != 0xBE)
		{
			corrupt = true;
			break;
		}
	}
	for (int i = 0; i < safetyEnd; i++)
	{
		if (data[blockSize - safetyEnd + i] != 0xAF)
		{
			corrupt = true;
			break;
		}
	}
	// Check the extra gap between 'block' and 'after'
	int offset = safetyBegin + allocSize;
	while ((offset & 0x0000000F) != 0)
	{
		if (data[offset++] != 0xAE)
		{
			corrupt = true;
			break;
		}
	}
	if (corrupt && (!quiet))
	{
		pprintf("Memory overwritten:\n");
		PTrace();
		pprintf("Safety areas:\n");
		pprintf("Before: ");
		for (int i = 0; i < safetyBegin; i++)
		{
			pprintf("%02X ", data[i]);
		}
		pprintf("\nExtra: ");
		int offset = safetyBegin + allocSize;
		while ((offset & 0x0000000F) != 0)
		{
			pprintf("%02X ", data[offset++]);
		}
		pprintf("\nAfter:  ");
		for (int i = 0; i < safetyEnd; i++)
		{
			pprintf("%02X ", data[blockSize - safetyEnd + i]);
		}
		pprintf("\n");
	}
	return !corrupt;
}

/**
 * list: List of all memory blocks, allocated or not. List is initialized in
 * the first Malloc call, but nothing should be freed before being allocated,
 * so this approach should be safe.
 */
static MemHeader * list = 0;
/**
 * freeList: List of all free blocks.
 */
static MemHeader * freeList = 0;

static void Free(void *ap)
{
//	pprintf("Free\n");
//	CheckMemory("Before Free\n");
	ASSERT(list);
	char * cp = (char *) ap;
	cp -= safetyBegin;
	MemHeader * header = (MemHeader *) cp;
	header--;
	ASSERT(header->line != -1);
	if (safetyTest)
		header->SafetyTest();
	SR cs = EnterCritical();
	header->file = "free";
	header->line = -1;
	header->allocSize = header->blockSize;
	bool merged = false;
	// Merge with previous block?
	if ((header->prev) && (header->prev->line == -1))
	{
		MemHeader * firstFree = header->prev;
		firstFree->next = header->next;
		firstFree->next->prev = firstFree;
		firstFree->blockSize += header->blockSize + sizeof(MemHeader);
		firstFree->allocSize = firstFree->blockSize;
		// Pointers back and forward from the previous block are already
		// in the right places, we only extended the free block by the new freed
		// space.
		header = firstFree;
		merged = true;
	}
	// Merge with next block?
	if ((header->next) && (header->next->line == -1))
	{
		MemHeader * nextFree = header->next;
		header->next = nextFree->next;
		if (nextFree->next)
			nextFree->next->prev = header;
		header->blockSize += nextFree->blockSize + sizeof(MemHeader);
		header->allocSize = header->blockSize;
		// Blocks are merged, but free list must also be updated.
		// First, if the block being merged has next free, this next's prev is
		// now our block
		if (nextFree->nextFree)
			nextFree->nextFree->prevFree = header;
		// Second, our next's next is now our next
		header->nextFree = nextFree->nextFree;
		// Here it is possible that we freed the block between two free ones and
		// as a result, we merge all 3 into one block
		if (nextFree->prevFree == header)
		{
			// Nothing more to do, because we merged with the block already in
			// the free list, links back are correct and links forward were
			// fixed earlier.
		}
		else
		{
			// Merged only with the next block. Next block was in the list.
			// Links back need changes because we merged with the block that was
			// already in the list, but list linked to the merged part of the
			// block. So, the merged block's back links are our new back links.
			if (nextFree->prevFree)
				nextFree->prevFree->nextFree = header;
			header->prevFree = nextFree->prevFree;
		}
		// Last, what if we merged with the next that was the first free block
		// at that time? Fix up free list head if so.
		if (freeList == nextFree)
			freeList = header;
		merged = true;
	}
	// Merging attempts are complete. If we merged anything with anything, the
	// free list head is appropriately updated. If not, check if our block is
	// before the free list so we need to update the head.
	if (!merged)
	{
		if (header < freeList)
		{
			// This block is not linked anywhere, all pointers are 0 because
			// this block was allocated and not merged.
			header->nextFree = freeList;
			if (freeList)
				freeList->prevFree = header;
			freeList = header;
		}
		else
		{
			// Merge it into the middle of list where block < header and
			// block->next > header or is 0
			// header - needs to be in the middle of free list, so we need to
			// insert the new block into free list.
			MemHeader * free1;
			for (free1 = freeList; free1; free1 = free1->nextFree)
			{
				// Find a block that has smaller address than the new free block
				// and it also has nextFree either 0 or past our block
				MemHeader * free2 = free1->nextFree;
				if ((free2) && (free2 < header))
					continue;
				// Free1 is block before, free2 is block after (or 0)
				// Fix up links from here to previous first
				free1->nextFree = header;
				header->prevFree = free1;
				// And then fix the links to the next block. Next block may be
				// zero!
				header->nextFree = free2;
				if (free2)
					free2->prevFree = header;
				break;
			}
		}
	}
	header->FillDeallocatedData();
	header->FillSafetyAreas(false);
	ExitCritical(cs);
//	CheckMemory("After Free\n");
}

static void InitMemory()
{
	extern char __HEAP_START[];
	extern char __HEAP_END[];

	list = (MemHeader *) __HEAP_START;
	list->file = "free";
	list->line = -1;
	list->blockSize = (__HEAP_END - __HEAP_START) - sizeof(MemHeader);
	list->allocSize = list->blockSize;
	list->prev = 0;
	list->next = 0;
	list->prevFree = 0;
	list->nextFree = 0;
	freeList = list;
	if (safetyTest)
	{
		list->FillSafetyAreas(false);
		list->FillDeallocatedData();
	}
//	CheckMemory("InitMemory\n");
}

static void * Malloc(size_t size, char * file, int line, bool init)
{
	SR cs = EnterCritical();
	if (!list)
		InitMemory();
//	pprintf("Malloc\n");
//	CheckMemory("Before Malloc\n");
	int allocSize = size;
	size += 15;
	size &= 0xFFFFFFF0;
	size += safetyBegin;
	size += safetyEnd;
	MemHeader * header;
	for (header = freeList; header; header = header->nextFree)
	{
		if ((header->blockSize >= (int)size) && (header->line == -1))
			break;
	}
	if (!header)
	{
		pprintf("Malloc(%d) failed, no free memory...\n", size);
		pprintf("Request from %s, line %d\n", file, line);
		CyGOS::WriteSystemEvent(ALLOCATION_ERROR, line ,((file[4] << 24) | (file[5] << 16) | (file[6] << 8) | file[7]),
					((file[0] << 24) | (file[1] << 16) | (file[2] << 8) | file[3]),
					 true);
		ASSERT(0);
		return 0;
	}
	int leftover = header->blockSize - size;
	int nextBlockSize = sizeof(MemHeader) + safetyBegin + safetyEnd + 16;
	if (leftover >= nextBlockSize)
	{
		// Leftover of free block is big enough to make another free block of
		// reasonable size.
		MemHeader * newFree = (MemHeader *) (((char *) header)
				+ sizeof(MemHeader) + size);
		newFree->file = "free";
		newFree->line = -1;
		newFree->blockSize = header->blockSize - sizeof(MemHeader) - size;
		newFree->allocSize = newFree->blockSize;
		newFree->prev = header;
		newFree->next = header->next;
		if (newFree->next)
			newFree->next->prev = newFree;
		header->next = newFree;
		header->blockSize = size;
		// new free block stays in same spot in the linked list as this block
		// was, since we just cut the part off from the front
		newFree->nextFree = header->nextFree;
		newFree->prevFree = header->prevFree;
		// also, fix the links from blocks before and after
		if (newFree->nextFree)
			newFree->nextFree->prevFree = newFree;
		if (newFree->prevFree)
			newFree->prevFree->nextFree = newFree;
		// And last thing, if the block was cut from the head of the list,
		// update the head as well.
		if (freeList == header)
			freeList = newFree;
		// this block is now allocated, so pointers are 0
		header->nextFree = 0;
		header->prevFree = 0;
		newFree->FillDeallocatedData();
		newFree->FillSafetyAreas(false);
	}
	else
	{
		// Whole block is allocated. All next/prev free pointers must be cross-
		// linked around this block since this block is not free any more.
		if (header->nextFree)
			header->nextFree->prevFree = header->prevFree;
		if (header->prevFree)
			header->prevFree->nextFree = header->nextFree;
		// If this happened to be the first block on free list, update the list
		// head too.
		if (freeList == header)
			freeList = header->nextFree;
		// This block is now allocated, so pointers are 0
		header->nextFree = 0;
		header->prevFree = 0;
	}
	header->file = file;
	header->line = line;
	header->allocSize = allocSize;

	if (safetyFill)
		header->FillSafetyAreas(init);
	unsigned char * data = header->data + safetyBegin;
	ExitCritical(cs);
//	CheckMemory("After Malloc\n");
	return data;
}

void ShowMemory()
{
	if (!list)
		InitMemory();
	pprintf("Memory contents:\n");
	MemHeader * header;
	unsigned int WdKick = 0;

	for (header = list; header; header = header->next)
	{
		header->PTrace();
		if((++WdKick & 0xFF) == 0xFF)
			CyGOS::KickWatchDog();
	}
	pprintf("\nFree list:\n");
	int count = 0;
	for (header = freeList; header; header = header->nextFree)
	{
		header->PTrace();
		if (count++ > 20)
		{
			pprintf("Aborting\n");
			break;
		}
		if((++WdKick & 0xFF) == 0xFF)
			CyGOS::KickWatchDog();
	}
	pprintf("\n\n");
}

/**
 * new[]: Allocate memory for an array of objects. Operator executes with
 * interrupts disabled.
 * @param length Region length to allocate.
 * @return void* Pointer to allocated memory, or 0.
 */
void * operator new[](size_t size, char * file, int line)
{
	return Malloc(size, file, line, false);
}

/**
 * new[]: Allocate memory for an array of objects. Operator executes with
 * interrupts disabled.
 * @param length Region length to allocate.
 * @return void* Pointer to allocated memory, or 0.
 */
void * operator new[](size_t size)
{
	return Malloc(size, "Unknown", 0, false);
}

/**
 * new: Allocate memory for a single object. Operator executes with
 * interrupts disabled.
 * @param length Region length to allocate.
 * @return void* Pointer to allocated memory, or 0.
 */
void * operator new(size_t size, char * file, int line)
{
	return Malloc(size, file, line, false);
}

/**
 * new: Allocate memory for a single object. Operator executes with
 * interrupts disabled.
 * @param length Region length to allocate.
 * @return void* Pointer to allocated memory, or 0.
 */
void * operator new(size_t size)
{
	return Malloc(size, "Unknown", 0, false);
}

/**
 * delete: Deallocate a memory block.
 * @param ptr Memory to deallocate.
 */
void operator delete(void * ptr)
{
	free(ptr);
}

void operator delete[](void * ptr)
{
   free(ptr);
}

/**
 * malloc: The malloc function allocates a memory block of at least size bytes.
 * The block may be larger than size bytes because of space required for
 * alignment and maintenance information.
 * @param size Bytes to allocate.
 * @return void * malloc returns a void pointer to the allocated space, or NULL
 * if there is insufficient memory available. To return a pointer to a type
 * other than void, use a type cast on the return value. The storage space
 * pointed to by the return value is guaranteed to be suitably aligned for
 * storage of any type of object. If size is 0, malloc allocates a zero-length
 * item in the heap and returns a valid pointer to that item.
 */
void * malloc(size_t size)
{
	return Malloc(size, "malloc", 0, false);
}

void * mallocNoInit(size_t size)
{
	return Malloc(size, "MallocAlignedNoCache", 0, false);
}

/**
 * calloc: The calloc function allocates storage space for an array of num
 * elements, each of length size bytes. Each element is initialized to 0.
 * @param num Number of elements.
 * @param size Length in bytes of each element.
 * @return void * calloc returns a pointer to the allocated space. The storage
 * space pointed to by the return value is guaranteed to be suitably aligned
 * for storage of any type of object. To get a pointer to a type other than
 * void, use a type cast on the return value.
 */
void * calloc(size_t num, size_t size)
{
	size *= num;
	void * ptr = malloc(size);
	if (ptr)
		memset(ptr, 0, size);
	return ptr;
}

void * Memrealloc(void * ptr, size_t size, char * file, int line)
{
	void * retMem = 0;
	if((ptr) && (size == 0))
	{
		free(ptr);
		return 0;
	}
	if((!ptr) && (size > 0))
	{
		return Malloc(size, file, line, false);
	}
	if(ptr)
	{
		char * cp = (char *) ptr;
		cp -= safetyBegin;
		MemHeader * header = (MemHeader *) cp;
		header--;
		retMem = Malloc(size, file, line, false);
		if((unsigned long)(header -> allocSize) < size)
			memcpy(retMem, ptr, header -> allocSize);
		else
			memcpy(retMem, ptr, size);

		free(ptr);
	}
	return retMem;
}
/**
 * free: The free function deallocates a memory block (memblock) that was
 * previously allocated by a call to calloc, malloc, or realloc. The number of
 * freed bytes is equivalent to the number of bytes requested when the block
 * was allocated (or reallocated, in the case of realloc). If memblock is NULL,
 * the pointer is ignored and free immediately returns. Attempting to free an
 * invalid pointer (a pointer to a memory block that was not allocated by
 * calloc, malloc, or realloc, or already freed area) may affect subsequent
 * allocation requests and cause errors. Also, aligned no-cache memory regions
 * cannot be freed through this function call.
 */
void free(void * ptr)
{
	if (ptr)
	{
		Free(ptr);
	}
}

/**
 * AllocAlignedNoCache: Allocate memory block at least the specified size in
 * non-cacheable area of main memory. Function uses Board::AllocAlignedNoCache
 * function.
 * @param size Size fo the memory block to allocate.
 * @param align Memory region alignment. If anything less than 16 is passed in
 * the parameter, 16 will be used. Parameter is also verified to be a power of
 * two.
 * @return void * A pointer to the aligned non-cacheable area of specified size.
 */
void * MallocAlignedNoCache(size_t size, int align)
{
	return Board::AllocAlignedNoCache(size, align);
}

/**
 * CallocAlignedNoCache: Allocate memory block at least the specified size in
 * non-cacheable area of main memory. Function uses Board::AllocAlignedNoCache
 * function. Function does not check if element size and alignment make sense
 * together.
 * @param number Number of elements.
 * @param size Length in bytes of each element.
 * @param align Memory region alignment. If anything less than 16 is passed in
 * the parameter, 16 will be used. Parameter is also verified to be a power of
 * two.
 * @return void * A pointer to the aligned non-cacheable area of specified size.
 */
void * CallocAlignedNoCache(size_t number, size_t size, int align)
{
	size *= number;
	void * ptr = Board::AllocAlignedNoCache(size, align);
	if (ptr)
		memset(ptr, 0, size);
	return ptr;
}

/**
 * FreeAlignedNoCache: Free a block of memory that was allocated as aligned
 * non-cacheable memory. Function uses Board::FreeAlignedNoCache function.
 * @param ptr Pointer returned by AllocAlignedNoCache function call.
 */
void FreeAlignedNoCache(void * ptr)
{
	if (ptr)
		Board::FreeAlignedNoCache(ptr);
}

/**
 * InitMemoryAllocation: Optional routine to specify diagnostics-related memory
 * allocation parameters. If routine is not called, all parameters are set to
 * defaults. Parameters specifying safety size must not be changed after first
 * memory block is allocated, because they affect translation from user data
 * pointer to memory header pointer.
 * @param before Safety area size before the user pointer. Default is 256 bytes.
 * @param after Safety area after the user pointer. Default is 256 bytes.
 * @param fill If non-zero, safety areas are filled with 0xBE and 0xAF patterns.
 * If zero, safety areas are allocated but not filled for performance.
 * @param test If non-zero, safety areas are verified to contain patterns 0xBE
 * or 0xAF. If test is non-zero, fill is forced non-zero.
 */
void InitMemoryAllocation(int before, int after, int fill, int test)
{
	safetyBegin = before;
	safetyEnd = after;
	safetyFill = fill;
	safetyTest = test;
	if (safetyTest)
		safetyFill = safetyTest;
}

/**
 * @return true if OK, false if corrupted.
 */
static bool MemoryCheck()
{
	if (!list)
		InitMemory();
	for (MemHeader * mem = list; mem; mem = mem->next)
	{
		if (!mem->SafetyTest())
			return false;
	}
	return true;
}

void CheckMemory(char * message)
{
	//pprintf(message);
	if (!MemoryCheck())
	{
		pprintf(" It Was ");
		pprintf(message);
		CRASH("memory");
	}
}
