//=============================================================================
//!
//! @file "BufferPool.c"
//!
//! @brief
//!
//! @copyright Copyright 2020 Laird
//!            All Rights Reserved.
//=============================================================================

//=============================================================================
// Includes
//=============================================================================
#include <string.h>
#include "Framework.h"

//=============================================================================
// Local Constant Definitions
//=============================================================================

//=============================================================================
// Type Definitions
//=============================================================================

//=============================================================================
// Local Data Definitions
//=============================================================================

K_MEM_POOL_DEFINE(bufferPool, BUFFER_POOL_MINSZ, BUFFER_POOL_MAXSZ,
		  BUFFER_POOL_NMAX, BUFFER_POOL_ALIGN);

static atomic_t takeFailed = ATOMIC_INIT(0);

//=============================================================================
// Local Function Prototypes
//=============================================================================

//=============================================================================
// Global Function Definitions
//=============================================================================

void BufferPool_Initialize(void)
{
	return;
}

void *BufferPool_TryToTake(size_t Size)
{
	int key = irq_lock();
	void *p = k_mem_pool_malloc(&bufferPool, Size);
	if (p != NULL) {
		memset(p, 0, Size);
	}
	irq_unlock(key);
	return p;
}

void *BufferPool_Take(size_t Size)
{
	int key = irq_lock();
	void *ptr = BufferPool_TryToTake(Size);

	if (ptr == NULL) {
		// Prevent recursive entry.
		if (atomic_get(&takeFailed) == 0) {
			atomic_set(&takeFailed, 1);
			FRAMEWORK_ASSERT(FORCED);
		}
	}
	irq_unlock(key);
	return ptr;
}

void BufferPool_Free(void *pBuffer)
{
	int key = irq_lock();
	k_free(pBuffer);
	irq_unlock(key);
}

//=============================================================================
// Local Function Definitions
//=============================================================================
// NA

// end
