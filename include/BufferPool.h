/**
 * @file BufferPool.h
 * @brief Allocates buffers for use with the message framework.
 *
 * Copyright (c) 2020 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BUFFER_POOL_H__
#define __BUFFER_POOL_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <stddef.h>

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/
/**
 * @brief Prepares buffer pool for use.
 */
void BufferPool_Initialize(void);

/**
 * @brief Allocates a buffer of at least Size bytes and returns a pointer.
 * The buffer is set to zero.
 * This function won't assert if a buffer can't be taken.
 */
void *BufferPool_TryToTake(size_t Size);

/**
 * @brief Allocates a buffer of at least Size bytes and returns a pointer.
 * The buffer is set to zero.
 * This function will assert if a buffer can't be taken.
 */
void *BufferPool_Take(size_t Size);

/**
 * @brief Puts a buffer back into a free buffer pool.
 */
void BufferPool_Free(void *pBuffer);

#ifdef __cplusplus
}
#endif

#endif /* __BUFFER_POOL_H__ */
