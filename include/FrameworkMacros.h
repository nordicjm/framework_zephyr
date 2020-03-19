/**
 * @file FrameworkMacros.h
 * @brief Macros for commonly performed actions.
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
/* Header and Allocation                                                      */
/******************************************************************************/
#define FRAMEWORK_MSG_HEADER_INIT(p, c, i)                                     \
	do {                                                                   \
		FRAMEWORK_ASSERT((p) != NULL);                                 \
		p->header.msgCode = (c);                                       \
		p->header.rxId = FWK_ID_RESERVED;                              \
		p->header.txId = (i);                                          \
	} while (0)

#define FRAMEWORK_MSG_DEALLOCATE_IF_UNSENT(status, pMsg)                       \
	do {                                                                   \
		if ((status) == FWK_ERROR) {                                   \
			BufferPool_Free(pMsg);                                 \
		}                                                              \
	} while (0)

/******************************************************************************/
/* Message Send                                                               */
/******************************************************************************/

/* Sends a message without a payload to oneself without using msg router */
#define FRAMEWORK_MSG_SEND_DIRECT_TO_SELF(task, code)                          \
	do {                                                                   \
		BaseType_t macroResult = FWK_ERROR;                            \
		FwkMsg_t *pMacroMsg =                                          \
			(FwkMsg_t *)BufferPool_Take(sizeof(FwkMsg_t));         \
		FRAMEWORK_ASSERT(pMacroMsg != NULL);                           \
		if (pMacroMsg != NULL) {                                       \
			FRAMEWORK_MSG_HEADER_INIT(pMacroMsg, (code), task.id); \
			pMacroMsg->header.rxId = task.id;                      \
			macroResult = Framework_Queue(task.pQueue, &pMacroMsg, \
						      K_NO_WAIT);              \
			FRAMEWORK_MSG_DEALLOCATE_IF_UNSENT(macroResult,        \
							   pMacroMsg);         \
			FRAMEWORK_ASSERT(macroResult == FWK_SUCCESS);          \
		}                                                              \
	} while (0)

#define FRAMEWORK_MSG_SEND(pMacroMsg)                                          \
	do {                                                                   \
		BaseType_t macroResult = Framework_Send(                       \
			(pMacroMsg)->header.rxId, (FwkMsg_t *)(pMacroMsg));    \
		FRAMEWORK_MSG_DEALLOCATE_IF_UNSENT(macroResult, (pMacroMsg));  \
		FRAMEWORK_ASSERT(macroResult == FWK_SUCCESS);                  \
	} while (0)

/* Try means do not assert if the queue is full. */
#define FRAMEWORK_MSG_TRY_TO_SEND(pMacroMsg)                                   \
	do {                                                                   \
		BaseType_t macroResult = Framework_Send(                       \
			(pMacroMsg)->header.rxId, (FwkMsg_t *)(pMacroMsg));    \
		FRAMEWORK_MSG_DEALLOCATE_IF_UNSENT(macroResult, (pMacroMsg));  \
	} while (0)

#define FRAMEWORK_MSG_SEND_TO(destId, pMacroMsg)                               \
	do {                                                                   \
		(pMacroMsg)->header.rxId = (destId);                           \
		BaseType_t macroResult = Framework_Send(                       \
			(pMacroMsg)->header.rxId, (FwkMsg_t *)(pMacroMsg));    \
		FRAMEWORK_MSG_DEALLOCATE_IF_UNSENT(macroResult, (pMacroMsg));  \
		FRAMEWORK_ASSERT(macroResult == FWK_SUCCESS);                  \
	} while (0)

#define FRAMEWORK_MSG_UNICAST(pMacroMsg)                                       \
	do {                                                                   \
		BaseType_t macroResult =                                       \
			Framework_Unicast((FwkMsg_t *)(pMacroMsg));            \
		FRAMEWORK_MSG_DEALLOCATE_IF_UNSENT(macroResult, (pMacroMsg));  \
		FRAMEWORK_ASSERT(macroResult == FWK_SUCCESS);                  \
	} while (0)

#define FRAMEWORK_MSG_UNICAST_CREATE_AND_SEND(txId, code)                      \
	do {                                                                   \
		BaseType_t macroResult = FWK_ERROR;                            \
		FwkMsg_t *pMacroMsg =                                          \
			(FwkMsg_t *)BufferPool_Take(sizeof(FwkMsg_t));         \
		FRAMEWORK_ASSERT(pMacroMsg != NULL);                           \
		if (pMacroMsg != NULL) {                                       \
			FRAMEWORK_MSG_HEADER_INIT(pMacroMsg, (code), (txId));  \
			macroResult = Framework_Unicast(pMacroMsg);            \
			FRAMEWORK_MSG_DEALLOCATE_IF_UNSENT(macroResult,        \
							   pMacroMsg);         \
			FRAMEWORK_ASSERT(macroResult == FWK_SUCCESS);          \
		}                                                              \
	} while (0)

#define FRAMEWORK_MSG_CREATE_AND_SEND(txId, rxId, code)                        \
	do {                                                                   \
		BaseType_t macroResult = FWK_ERROR;                            \
		FwkMsg_t *pMacroMsg =                                          \
			(FwkMsg_t *)BufferPool_Take(sizeof(FwkMsg_t));         \
		FRAMEWORK_ASSERT(pMacroMsg != NULL);                           \
		if (pMacroMsg != NULL) {                                       \
			FRAMEWORK_MSG_HEADER_INIT(pMacroMsg, (code), (txId));  \
			macroResult = Framework_Send((rxId), pMacroMsg);       \
			FRAMEWORK_MSG_DEALLOCATE_IF_UNSENT(macroResult,        \
							   pMacroMsg);         \
			FRAMEWORK_ASSERT(macroResult == FWK_SUCCESS);          \
		}                                                              \
	} while (0)

#define FRAMEWORK_MSG_CREATE_AND_BROADCAST(id, code)                           \
	do {                                                                   \
		size_t macroMsgSize = sizeof(FwkMsg_t);                        \
		FwkMsg_t *pMacroMsg = BufferPool_Take(macroMsgSize);           \
		if (pMacroMsg != NULL) {                                       \
			pMacroMsg->header.msgCode = (code);                    \
			pMacroMsg->header.txId = (id);                         \
			pMacroMsg->header.rxId = FRAMEWORK_TASK_ID_RESERVED;   \
			BaseType_t macroStatus = Framework_Broadcast(          \
				(FwkMsg_t *)pMacroMsg, macroMsgSize);          \
			if (macroStatus != FWK_SUCCESS) {                      \
				BufferPool_Free(pMacroMsg);                    \
			}                                                      \
		}                                                              \
	} while (0)

#define FRAMEWORK_MSG_REPLY(pMacroMsg, code)                                   \
	do {                                                                   \
		FwkMsgId_t swap = (pMacroMsg)->header.rxId;                    \
		(pMacroMsg)->header.rxId = (pMacroMsg)->header.txId;           \
		(pMacroMsg)->header.txId = swap;                               \
		(pMacroMsg)->header.msgCode = (code);                          \
		BaseType_t macroResult = Framework_Send(                       \
			(pMacroMsg)->header.rxId, (FwkMsg_t *)(pMacroMsg));    \
		FRAMEWORK_MSG_DEALLOCATE_IF_UNSENT(macroResult, (pMacroMsg));  \
		FRAMEWORK_ASSERT(macroResult == FWK_SUCCESS);                  \
	} while (0)

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_MACROS_H__ */
