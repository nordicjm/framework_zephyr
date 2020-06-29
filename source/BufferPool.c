/**
 * @file BufferPool.c
 * @brief
 *
 * Copyright (c) 2020 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define FWK_FNAME "BufferPool"

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <string.h>
#include "Framework.h"

/******************************************************************************/
/* Local Data Definitions                                                     */
/******************************************************************************/
K_HEAP_DEFINE(bufferPool, CONFIG_BUFFER_POOL_SIZE);

static atomic_t takeFailed = ATOMIC_INIT(0);

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
void BufferPool_Initialize(void)
{
	return; /* Everthing is done by K_HEAP_DEFINE */
}

void *BufferPool_TryToTakeTimeout(size_t Size, k_timeout_t Timeout)
{
	void *p = k_heap_alloc(&bufferPool, Size, Timeout);
	if (p != NULL) {
		memset(p, 0, Size);
	}
	return p;
}

void *BufferPool_TryToTake(size_t Size)
{
	return BufferPool_TryToTakeTimeout(Size, K_NO_WAIT);
}

void *BufferPool_Take(size_t Size)
{
	void *ptr = BufferPool_TryToTake(Size);
	if (ptr == NULL) {
		/* Prevent recursive entry. */
		if (atomic_get(&takeFailed) == 0) {
			atomic_set(&takeFailed, 1);
			FRAMEWORK_ASSERT(FORCED);
		}
	}
	return ptr;
}

void BufferPool_Free(void *pBuffer)
{
	k_heap_free(&bufferPool, pBuffer);
}
