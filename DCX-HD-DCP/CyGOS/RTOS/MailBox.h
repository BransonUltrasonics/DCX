// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/RTOS/MailBox.h_v   1.0   29 Oct 2010 10:39:42   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines inter-task communication and synchronization objects.

#ifndef MAILBOX_H_
#define MAILBOX_H_

#include "Fifo.h"

struct MailBox
{
public:
	MailBox(int size);
	bool TryFetch(void ** ptr);
	bool TryPost(void * ptr);
	bool Fetch(void ** ptr, int timeoutMs);
	bool Post(void * ptr, int timeoutMs);
protected:
	Fifo<void *> queue;
};

#endif
