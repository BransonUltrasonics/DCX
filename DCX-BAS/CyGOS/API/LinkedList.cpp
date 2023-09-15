// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/API/LinkedList.cpp_v   1.0   21 Mar 2012 12:25:24   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements linked list base class.

#include "LinkedList.h"
#include "DebugNew.h"

/**
 * LinkedListBase: Initialize the linked list base object.
 */
LinkedListBase::LinkedListBase()
{
	head = 0;
	tail = 0;
}

/**
 * ~LinkedListBase: Delete all resources allocated by the object. All nodes in
 * the linked list are removed. Function expects all nodes to be allocated
 * using <b>new</b> operator. Each node may contain inner data allocated using
 * aligned non-cached allocation functions if necessary.
 */
LinkedListBase::~LinkedListBase()
{
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		POSITION remove = pos;
		GetNext(pos);
		RemoveAt(remove);
	}
}

/**
 * AddHead: Add a node to the head of the linked list.
 * @param data Data to put into this new node.
 */
void LinkedListBase::AddHead(void * data)
{
	ListNode * node = new ListNode(data, 0, head);
	if (head)
		head->prev = node;
	else
		tail = node;
	head = node;
}

/**
 * AddTail: Add a node to the tail of the linked list.
 * @param data Data to put into this new node.
 */
void LinkedListBase::AddTail(void * data)
{
	ListNode * node = new ListNode(data, tail, 0);
	if (tail)
		tail->next = node;
	else
		head = node;
	tail = node;
}

void * LinkedListBase::GetNext(POSITION & pos) const
{
	ASSERT(pos);
	ListNode * node = (ListNode *) pos;
	pos = (POSITION) node->next;
	return node->data;
}

POSITION LinkedListBase::Find(void * data) const
{
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		if (GetAt(pos) == data)
			break;
		GetNext(pos);
	}
	return pos;
}

void LinkedListBase::RemoveAt(POSITION pos)
{
	ASSERT(pos);
	ListNode * node = (ListNode *) pos;
	if (node == head)
		head = node->next;
	else
		node->prev->next = node->next;
	if (node == tail)
		tail = node->prev;
	else
		node->next->prev = node->prev;
	delete node;
}

int LinkedListBase::GetCount() const
{
	int count = 0;
	ListNode * node = head;
	while (node)
	{
		count++;
		node = node->next;
	}
	return count;
}

void * LinkedListBase::GetHead() const
{
	if (head)
		return head->data;
	return 0;
}

void * LinkedListBase::GetTail() const
{
	if (tail)
		return tail->data;
	return 0;
}

void * LinkedListBase::RemoveHead()
{
	if (!head)
		return 0;
	void * data = head->data;
	RemoveAt(GetHeadPosition());
	return data;
}

void * LinkedListBase::RemoveTail()
{
	if (!tail)
		return 0;
	void * data = tail->data;
	RemoveAt(GetTailPosition());
	return data;
}
