//
//  Copyright (C) 2004-2007 by Haris Kurtagic
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//


// Number of threads in pool 

#define POOL_THREADS 4

// Number of entries in ECBqueue 

#define WORK_QUEUE_ENTRIES 16

/* Global critical section to control access to ECB queue */

extern CRITICAL_SECTION csQueueLock;

/* 
	Semaphore to wait on in worker thread; each time an ECB is added to the 
	ECBqueue by HttpExtensionProc, the semaphore must be released once
*/

extern HANDLE hWorkSem;

/*
	These functions will add/retrieve an ECB to/from the linked list.
	ENTER csQueueLock BEFORE CALLING AND LEAVE csQueueLock AFTER
	RETURNING FROM THESE FUNCTIONS!!!
*/

BOOL AddWorkQueueEntry(EXTENSION_CONTROL_BLOCK *);
BOOL GetWorkQueueEntry(EXTENSION_CONTROL_BLOCK **ppECB);

/* This function initializes the thread pool */

BOOL InitThreadPool(void);

/* Function that threads in pool run */

DWORD WINAPI WorkerFunction(LPVOID); 
