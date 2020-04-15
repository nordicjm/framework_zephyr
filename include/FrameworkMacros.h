/**
 * @file FrameworkMacros.h
 * @brief Macros for commonly performed actions.
 * @note FrameworkMsg.h - Functions underneath the macros.
 *
 * Copyright (c) 2020 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __FRAMEWORK_MACROS_H__
#define __FRAMEWORK_MACROS_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#ifndef __FRAMEWORK_H__
#error "One cannot use the macros without the Framework"
#endif

/******************************************************************************/
/* Macro Definitions                                                          */
/******************************************************************************/
/**
 * @param rxId - The destination (receiver) of the message.
 * @param txId - The source of the message
 * @param code - The message code
 * @param pMacroMsg - A previously created message that is ready to send.
 */

#define FRAMEWORK_MSG_SEND_TO_SELF(rxId, code)                                 \
	FwkMsg_CreateAndSendToSelf(rxId, code)

#define FRAMEWORK_MSG_SEND(pMacroMsg) FwkMsg_Send((FwkMsg_t *)pMacroMsg)

#define FRAMEWORK_MSG_TRY_TO_SEND(pMacroMsg)                                   \
	FwkMsg_TryToSend((FwkMsg_t *)pMacroMsg)

#define FRAMEWORK_MSG_SEND_TO(destId, pMacroMsg)                               \
	FwkMsg_SendTo((FwkMsg_t *)pMacroMsg, destId)

#define FRAMEWORK_MSG_UNICAST(pMacroMsg) FwkMsg_Unicast((FwkMsg_t *)pMacroMsg)

#define FRAMEWORK_MSG_UNICAST_CREATE_AND_SEND(txId, code)                      \
	FwkMsg_UnicastCreateAndSend(txId, code)

#define FRAMEWORK_MSG_CREATE_AND_SEND(txId, rxId, code)                        \
	FwkMsg_CreateAndSend(txId, rxId, code)

#define FRAMEWORK_MSG_CREATE_AND_BROADCAST(id, code)                           \
	FwkMsg_CreateAndBroadcast(id, code)

#define FRAMEWORK_MSG_REPLY(pMacroMsg, code)                                   \
	FwkMsg_Reply((FwkMsg_t *)pMacroMsg, code)

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_MACROS_H__ */
