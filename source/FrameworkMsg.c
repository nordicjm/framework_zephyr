/**
 * @file FrameworkMsg.c
 *
 * Copyright (c) 2020 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include "Framework.h"

/******************************************************************************/
/* Local Function Prototypes                                                  */
/******************************************************************************/
static void DeallocateOnError(FwkMsg_t *pMsg, BaseType_t status);

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
void FwkMsg_Send(FwkMsg_t *pMsg)
{
	BaseType_t result = Framework_Send(pMsg->header.rxId, pMsg);
	DeallocateOnError(pMsg, result);
	FRAMEWORK_ASSERT(result == FWK_SUCCESS);
}

void FwkMsg_TryToSend(FwkMsg_t *pMsg)
{
	BaseType_t result = Framework_Send(pMsg->header.rxId, pMsg);
	DeallocateOnError(pMsg, result);
}

void FwkMsg_SendTo(FwkMsg_t *pMsg, FwkId_t DestId)
{
	pMsg->header.rxId = DestId;
	BaseType_t result = Framework_Send(pMsg->header.rxId, pMsg);
	DeallocateOnError(pMsg, result);
	FRAMEWORK_ASSERT(result == FWK_SUCCESS);
}

void FwkMsg_Unicast(FwkMsg_t *pMsg)
{
	BaseType_t result = Framework_Unicast(pMsg);
	DeallocateOnError(pMsg, result);
	FRAMEWORK_ASSERT(result == FWK_SUCCESS);
}

void FwkMsg_CreateAndSend(FwkId_t TxId, FwkId_t RxId, FwkMsgCode_t Code)
{
	FwkMsg_t *pMsg = (FwkMsg_t *)BufferPool_Take(sizeof(FwkMsg_t));
	FRAMEWORK_ASSERT(pMsg != NULL);
	if (pMsg != NULL) {
		FRAMEWORK_MSG_HEADER_INIT(pMsg, Code, TxId);
		BaseType_t result = Framework_Send(RxId, pMsg);
		DeallocateOnError(pMsg, result);
		FRAMEWORK_ASSERT(result == FWK_SUCCESS);
	}
}

void FwkMsg_CreateAndSendToSelf(FwkId_t Id, FwkMsgCode_t Code)
{
	FwkMsg_t *pMsg = (FwkMsg_t *)BufferPool_Take(sizeof(FwkMsg_t));
	FRAMEWORK_ASSERT(pMsg != NULL);
	if (pMsg != NULL) {
		FRAMEWORK_MSG_HEADER_INIT(pMsg, Code, Id);
		pMsg->header.rxId = Id;
		BaseType_t result = Framework_Send(Id, pMsg);
		DeallocateOnError(pMsg, result);
		FRAMEWORK_ASSERT(result == FWK_SUCCESS);
	}
}

void FwkMsg_UnicastCreateAndSend(FwkId_t TxId, FwkMsgCode_t Code)
{
	FwkMsg_t *pMsg = (FwkMsg_t *)BufferPool_Take(sizeof(FwkMsg_t));
	FRAMEWORK_ASSERT(pMsg != NULL);
	if (pMsg != NULL) {
		FRAMEWORK_MSG_HEADER_INIT(pMsg, Code, TxId);
		BaseType_t result = Framework_Unicast(pMsg);
		DeallocateOnError(pMsg, result);
		FRAMEWORK_ASSERT(result == FWK_SUCCESS);
	}
}

void FwkMsg_CreateAndBroadcast(FwkId_t TxId, FwkMsgCode_t Code)
{
	size_t size = sizeof(FwkMsg_t);
	FwkMsg_t *pMsg = BufferPool_Take(size);
	if (pMsg != NULL) {
		FRAMEWORK_MSG_HEADER_INIT(pMsg, Code, TxId);
		pMsg->header.rxId = FWK_ID_RESERVED;
		BaseType_t result = Framework_Broadcast(pMsg, size);
		DeallocateOnError(pMsg, result);
	}
}

void FwkMsg_Reply(FwkMsg_t *pMsg, FwkMsgCode_t Code)
{
	FwkId_t swap = pMsg->header.rxId;
	pMsg->header.rxId = pMsg->header.txId;
	pMsg->header.txId = swap;
	pMsg->header.msgCode = Code;
	BaseType_t result = Framework_Send(pMsg->header.rxId, pMsg);
	DeallocateOnError(pMsg, result);
	FRAMEWORK_ASSERT(result == FWK_SUCCESS);
}

/******************************************************************************/
/* Local Function Definitions                                                 */
/******************************************************************************/
static void DeallocateOnError(FwkMsg_t *pMsg, BaseType_t status)
{
	if (status != FWK_SUCCESS) {
		BufferPool_Free(pMsg);
	}
}
