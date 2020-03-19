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
/* Global Constants, Macros and Type Definitions                              */
/******************************************************************************/
typedef enum FwkMsgCodeEnum {
	/* Reserved by Framework (DO NOT DELETE) */
	FMC_INVALID = 0,
	FMC_PERIODIC,
	FMC_SOFTWARE_RESET,
	FMC_WATCHDOG_CHALLENGE,
	FMC_WATCHDOG_RESPONSE,

	/* Application Specific */

	/* Last value (DO NOT DELETE) */
	NUMBER_OF_FRAMEWORK_MSG_CODES
} FwkMsgCode_t;


#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_MSG_CODES_H__ */
