/**
 * @file FrameworkMsg.h
 * @brief Helper/Wrapper functions for sending framework messages.
 *
 * If enabled, functions assert on error.
 * They always deallocate messages on error.
 *
 * @note In FrameworkMacros.h these functions are renamed to be
 * compatible with previous revisions of the framework.
 *
 * Copyright (c) 2020 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __FRAMEWORK_MSG_H__
#define __FRAMEWORK_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#ifndef __FRAMEWORK_H__
#error "One cannot use these functions without the Framework"
#endif

/******************************************************************************/
/* Local Constant, Macro and Type Definitions                                 */
/******************************************************************************/
#define FRAMEWORK_MSG_HEADER_INIT(p, c, i)                                     \
	do {                                                                   \
		FRAMEWORK_ASSERT((p) != NULL);                                 \
		p->header.msgCode = (c);                                       \
		p->header.rxId = FWK_ID_RESERVED;                              \
		p->header.txId = (i);                                          \
	} while (0)

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/

/**
 * @brief Wrapper for Framework_Send
 *
 * @param pMsg pointer to a framework message
 */
void FwkMsg_Send(FwkMsg_t *pMsg);

/**
 * @brief Wrapper for Framework_Send that doesn't assert if dest queue is full.
 *
 * @param pMsg pointer to a framework message
 */
void FwkMsg_TryToSend(FwkMsg_t *pMsg);

/**
 * @brief Wrapper for Framework_Send when used with FRAMEWORK_MSG_HEADER_INIT.
 *
 * @param pMsg pointer to a framework message
 * @param DestId is used to set pMsg->header.rxId
 */
void FwkMsg_SendTo(FwkMsg_t *pMsg, FwkId_t DestId);

/**
 * @brief Wrapper for Framework_Unicast.
 *
 * @param pMsg pointer to a framework message
 */
void FwkMsg_Unicast(FwkMsg_t *pMsg);

/**
 * @brief Allocates message from buffer pool and sends it using Framework_Send.
 *
 * @param TxId source of message
 * @param RxId destination of message
 * @param Code message type
 */
void FwkMsg_CreateAndSend(FwkId_t TxId, FwkId_t RxId, FwkMsgCode_t Code);

/**
 * @brief Shorter form of FwkMsg_CreateAndSend that can be used by a task to
 * send a message to itself.
 *
 * @param Id source and destination of message
 * @param Code message type
 */
void FwkMsg_CreateAndSendToSelf(FwkId_t Id, FwkMsgCode_t Code);

/**
 * @brief Allocates message from buffer pool and sends it using
 * Framework_Unicast.
 *
 * @param TxId source of message
 * @param Code message type
 */
void FwkMsg_UnicastCreateAndSend(FwkId_t TxId, FwkMsgCode_t Code);

/**
 * @brief Allocates message from buffer pool and broadcasts it using
 * Framework_Broadcast.
 *
 * @param TxId source of message
 * @param Code message type
 */
void FwkMsg_CreateAndBroadcast(FwkId_t TxId, FwkMsgCode_t Code);

/**
 * @brief Swaps RxId and txId, changes messsage code, and then sends using
 * Framework_Send.
 *
 * @note Often used with DISPATCH_DO_NOT_FREE.
 * @note The original sender must populate the txId.
 *
 * @param pMsg pointer to a framework message
 * @param Code message type
 */
void FwkMsg_Reply(FwkMsg_t *pMsg, FwkMsgCode_t Code);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_MSG_H__ */
