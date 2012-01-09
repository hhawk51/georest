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


#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <httpext.h>
#include "threadpool.h"


/* Global critical section to control access to ECB queue */

CRITICAL_SECTION csQueueLock;

/* 
Semaphore to wait on in worker thread; each time an ECB is added to the 
g_ECBqueue by HttpExtensionProc, the semaphore must be released once
*/

HANDLE hWorkSem;

/* Structure to create simple linked list */

typedef struct {
	EXTENSION_CONTROL_BLOCK *pECB;	/* Data for list entry */
	DWORD dwNextEntry;							/* Pointer to next entry */
} ECB_QUEUE_ENTRY;

/* Array that is a simple linked list */

ECB_QUEUE_ENTRY g_ECBqueue[WORK_QUEUE_ENTRIES];

HANDLE g_Threads[POOL_THREADS];

/* Index of next g_ECBqueue entry to use, and last Entry in use. */

DWORD dwCurrentEntry, dwLastEntry;

/* Flag to indicate that there are no other entries in the g_ECBqueue */

BOOL fQueueEmpty;

/*
	Description:

		Initialize our thread pool
*/

BOOL InitThreadPool(void)
{
	DWORD i;
	DWORD dwThreadID;

	/* Create Semaphore in nonsignaled state */

	if ((hWorkSem = CreateSemaphore(NULL, 0, 0x7fffffff, NULL)) == NULL)
		return FALSE;

	InitializeCriticalSection(&csQueueLock);

	fQueueEmpty = TRUE;

	/* Create Pool Threads */

	for (i = 0; i < POOL_THREADS; i++)
	{
	  g_Threads[i] = CreateThread(NULL, 0, WorkerFunction, (LPVOID)i, 0, &dwThreadID);			
	}

	/* Clear work queue */

	ZeroMemory(g_ECBqueue, WORK_QUEUE_ENTRIES * sizeof(ECB_QUEUE_ENTRY));

	return TRUE;
}

void CloseThreadPool(void)
{

  /* Create Pool Threads */

  for (int i = 0; i < POOL_THREADS; i++)
  {
    if( g_Threads[i] != NULL ) CloseHandle(g_Threads[i]);
  }

  return;
}

/*
	Description:

		Add single work unit to the queue

	Arguments:

		pECB - pointer to the extension control block

	Returns:

		TRUE if the unit was successfully queued
		FALSE if queue is full
*/

BOOL AddWorkQueueEntry(IN EXTENSION_CONTROL_BLOCK *pECB)
{
	DWORD i;

	for (i = 0; i < WORK_QUEUE_ENTRIES; i++) {

		if (g_ECBqueue[i].pECB == NULL) 
		{

			if (fQueueEmpty) 
			{

				dwCurrentEntry = i;

				fQueueEmpty = FALSE;

			} 
			else 
			{

				g_ECBqueue[dwLastEntry].dwNextEntry = i;
			}

			g_ECBqueue[i].pECB = pECB;

			dwLastEntry = i;

			return TRUE;
		}
	}

	/* If no NULL queue entry found, indicate failure */

	return FALSE;
}

BOOL GetWorkQueueEntry(OUT EXTENSION_CONTROL_BLOCK **ppECB)
{
	if ((*ppECB = g_ECBqueue[dwCurrentEntry].pECB) == NULL) 
	{

		return FALSE;
 
	} 
	else 
	{

		g_ECBqueue[dwCurrentEntry].pECB = NULL;

		if (dwCurrentEntry == dwLastEntry)	/* If this is only pending item */
			fQueueEmpty = TRUE;
		else
			dwCurrentEntry = g_ECBqueue[dwCurrentEntry].dwNextEntry;
	}

	return TRUE;
}
