// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/API/LinkedList.h_v   1.0   18 Aug 2014 09:15:24   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares linked list classes.

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "assert.h"
#include "stdio.h"
#include "DebugNew.h"

typedef void * POSITION;

/**
 * class ListNode: This is the base class for linked list nodes that are
 * managed by LinkedList template class. This class defines the linked list
 * 'next' pointer to link all nodes together. Templated LinkedList class
 * expects all nodes to be derived from this base class.
 */
class ListNode
{
	friend class LinkedListBase;
private:
	/**
	 * next: Pointer to the next node in linked list.
	 */
	ListNode * next;
	/**
	 * prev: Pointer to the previous node in linked list.
	 */
	ListNode * prev;
	/**
	 * data: Pointer to the data stored in that node.
	 */
	void * data;
	/**
	 * ListNode: The constructor to quickly fill up the node fields.
	 * @param d Data pointer.
	 * @param p Previous node pointer.
	 * @param n Next node pointer.
	 */
	ListNode(void * d, ListNode * p, ListNode * n)
	{
		data = d;
		prev = p;
		next = n;
	}
};

/**
 * class LinkedListBase: The base class for linked list. Class implements all
 * functionality that is not dependent on actual type of list nodes, such as
 * adding and removal of nodes, etc.
 */
class LinkedListBase
{
public:
	POSITION GetHeadPosition() const
	{
		return (POSITION) head;
	}
	POSITION GetTailPosition() const
	{
		return (POSITION) tail;
	}
	void RemoveAt(POSITION pos);
	void * RemoveHead();
	void * RemoveTail();
	void Print() const;
	int GetCount() const;
	static void Test();
protected:
	LinkedListBase();
	~LinkedListBase();
	void AddHead(void * data);
	void AddTail(void * data);
	void * GetHead() const;
	void * GetTail() const;
	void * GetNext(POSITION & pos) const;
	POSITION Find(void * data) const;
	void * GetAt(POSITION pos) const
	{
		ListNode * node = (ListNode *) pos;
		return node->data;
	}
	void TestGetNext();
	/**
	 * head: Pointer to the first node in the linked list, or 0 if list is
	 * empty.
	 */
	ListNode * head;
	/**
	 * tail: Pointer to the last node in the linked list, or 0 if list is
	 * empty.
	 */
	ListNode * tail;
};

/**
 * template <class Node> class LinkedList: Templated class provides cast-free
 * access to functions that return list node pointers. Linked list contains
 * objects derived from ListNode class.
 */
template<class Data> class PtrList: public LinkedListBase
{
public:
	/**
	 * PtrList: Initialize the linked list object. There is nothing to do
	 * for the template class object initialization that was not done in the
	 * base class constructor. This constructor is defined here because it is
	 * public and base class constructor is protected so no objects should be
	 * created using the base class.
	 */
	PtrList()
	{
	}
	void AddHead(Data * data)
	{
		LinkedListBase::AddHead(data);
	}
	void AddTail(Data * data)
	{
		LinkedListBase::AddTail(data);
	}
	Data * GetHead() const
	{
		return (Data *) LinkedListBase::GetHead();
	}
	Data * GetTail() const
	{
		return (Data *) LinkedListBase::GetTail();
	}
	Data * RemoveHead()
	{
		return (Data *) LinkedListBase::RemoveHead();
	}
	Data * RemoveTail()
	{
		return (Data *) LinkedListBase::RemoveTail();
	}
	Data * GetNext(POSITION & pos) const
	{
		return (Data *) LinkedListBase::GetNext(pos);
	}
	Data * GetAt(POSITION pos) const
	{
		return (Data *) LinkedListBase::GetAt(pos);
	}
	/**
	 * DeleteAt: Delete data and list node at specified position. Procedure can
	 * be used in GetHeadPosition()/GetNext() loop only if position before
	 * calling GetNext() is saved and then used for deleting the element. The
	 * reason for saving the position is that while GetNext() returns data at
	 * current position, it updates the position to point at next node, and to
	 * delete the current node data we need position that was used before
	 * GetNext() was called.
	 * @param pos Position in the list to delete.
	 */
	void DeleteAt(POSITION pos)
	{
		ASSERT(pos);
		Data * data = (Data *) GetAt(pos);
		delete data;
		RemoveAt(pos);
	}

	POSITION Find(Data * data) const
	{
		return LinkedListBase::Find(data);
	}
	/**
	 * FindDelete: Find data with matching pointer, delete the data and
	 * associated node. Position in the list is not updated.
	 * @param data Data to delete from the list and to deallocate memory.
	 */
	void FindDelete(Data * data)
	{
		POSITION pos = Find(data);
		if (pos)
			RemoveAt(pos);
		delete data;
	}
};

#endif
