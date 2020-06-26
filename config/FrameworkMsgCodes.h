/**
 * @file FrameworkMsgCodes.h
 * @brief Defines the couple of messages reserved by Framework.
 * Application message types can also be defined here.
 *
 * Copyright (c) 2020 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __FRAMEWORK_MSG_CODES_H__
#define __FRAMEWORK_MSG_CODES_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include "Framework.h"

/******************************************************************************/
/* Global Constants, Macros and Type Definitions                              */
/******************************************************************************/
enum FwkMsgCodeEnum {
	/* The first application specific message should be assigned the value
	 * FMC_APPLICATION_SPECIFIC_START.  There are codes reserved by framework.
	 */
	FMC_ADV = FMC_APPLICATION_SPECIFIC_START,

	/* Last value (DO NOT DELETE) */
	NUMBER_OF_FRAMEWORK_MSG_CODES
};
BUILD_ASSERT(sizeof(enum FwkMsgCodeEnum) <= sizeof(FwkMsgCode_t),
		 "Too many message codes");


#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_MSG_CODES_H__ */
