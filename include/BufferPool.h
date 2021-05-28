/**
 * @file BufferPool.h
 * @brief Allocates buffers for use with the message framework.
 *
 * Copyright (c) 2021 Laird Connectivity
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
#include <kernel.h>
#include <stddef.h>

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/
struct bp_stats {
	bool initialized;
	int space_available;
	int min_space_available;
	int min_size;
	int max_size;
	int allocs;
	int cur_allocs;
	int max_allocs;
	int take_failures;
	int last_fail_size;
#if CONFIG_BUFFER_POOL_WINDOW_SIZE > 0
	size_t windex;
	uint16_t window[CONFIG_BUFFER_POOL_WINDOW_SIZE];
#endif
};

#define BP_CONTEXT_UNUSED "NA"

#define BP_TRY_TO_TAKE(s) BufferPool_TryToTake(s, __func__)

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/
/**
 * @brief Prepares buffer pool for use.
 */
void BufferPool_Initialize(void);

/**
 * @brief Waits up to timeout to allocate a buffer of at least
 * size bytes and returns a pointer.
 * The buffer is set to zero.
 * This function won't assert if a buffer can't be taken.
 *
 * @param size in bytes
 * @param timeout zephyr timeout
 * @param context for printing warning when buffer can't be allocated
 * @return void*
 */
void *BufferPool_TryToTakeTimeout(size_t size, k_timeout_t timeout,
				  const char *const context);

/**
 * @brief Allocates a buffer of at least size bytes and returns a pointer.
 * The buffer is set to zero.
 * This function won't assert if a buffer can't be taken.
 *
 * @param size in bytes
 * @param context for printing warning when buffer can't be allocated
 * @return void*
 */
void *BufferPool_TryToTake(size_t size, const char *const context);

/**
 * @brief Allocates a buffer of at least size bytes and returns a pointer.
 * The buffer is set to zero.
 * This function will assert if a buffer can't be taken.
 */
void *BufferPool_Take(size_t size);

/**
 * @brief Put a buffer back into the free pool.
 */
void BufferPool_Free(void *pBuffer);

/**
 * @brief Get pointer to buffer pool statistics
 *
 * @param index of buffer pool.  Only 0 is valid at this time.
 *
 * @return struct bp_stats* NULL if index isn't valid
 */
struct bp_stats *BufferPool_GetStats(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif /* __BUFFER_POOL_H__ */
