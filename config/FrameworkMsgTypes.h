/**
 * @file FrameworkMessageTypes.h
 * @brief Project specific message types are defined here.
 *
 * Copyright (c) 2020 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __FRAMEWORK_MSG_TYPES_H__
#define __FRAMEWORK_MSG_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include "FrameworkMsgConfiguration.h"

/******************************************************************************/
/* Global Constants, Macros and Type Definitions                              */
/******************************************************************************/

/* The size of the buffer pool messages limits framework message sizes. */
#ifndef BUFFER_POOL_MINSZ
#define BUFFER_POOL_MINSZ 4
#endif

#ifndef BUFFER_POOL_MAXSZ
#define BUFFER_POOL_MAXSZ 4096
#endif

#ifndef BUFFER_POOL_NMAX
#define BUFFER_POOL_NMAX 8
#endif

#ifndef BUFFER_POOL_ALIGN
#define BUFFER_POOL_ALIGN 4
#endif

#define CHECK_FWK_MSG_SIZE(x)                                                  \
	BUILD_ASSERT_MSG(sizeof(x) <= BUFFER_POOL_MAXSZ,                       \
			 "Buffer Pool Max Message size is too small")

/******************************************************************************/
/* Project Specific Message Types                                             */
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_MSG_TYPES_H__ */
