//=============================================================================
//! @file BufferPool.h
//!
//! @brief
//!
//! @copyright Copyright 2020 Laird
//!            All Rights Reserved.
//=============================================================================

#ifndef BUFFER_POOL_H
#define BUFFER_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Includes
//=============================================================================
// NA

//=============================================================================
// Global Constants, Macros and Type Definitions
//=============================================================================
// NA

//=============================================================================
// Global Data Definitions
//=============================================================================
// NA

//=============================================================================
// Global Function Prototypes
//=============================================================================
// NA

//-----------------------------------------------
//! @brief  Allocates memory for the buffers
//!
void BufferPool_Initialize(void);

//-----------------------------------------------
//! @brief Allocates a buffer of at least Size bytes and returns a pointer.
//! The buffer is set to zero.
//! This function won't assert if a buffer can't be taken.
//!
void *BufferPool_TryToTake(size_t Size);

//-----------------------------------------------
//! @brief Allocates a buffer of at least Size bytes and returns a pointer.
//! The buffer is set to zero.
//! This function will assert if a buffer can't be taken.
//!
void *BufferPool_Take(size_t Size);

//-----------------------------------------------
//! @brief Puts a buffer back into a free buffer pool.
//!
void BufferPool_Free(void *pBuffer);

#ifdef __cplusplus
}
#endif

#endif

// end
