/**
 * @file Framework.c
 * @brief The message framework uses queues to pass pointers to messages
 * between tasks or from an interrupt or non-msg task to a message task.
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
/* Local Constant, Macro and Type Definitions                                 */
/******************************************************************************/
typedef struct MsgTaskArrayEntry {
	FwkMsgReceiver_t *pMsgReceiver;
	bool inUse;
} MsgTaskArrayEntry_t;

/******************************************************************************/
/* Local Function Prototypes                                                  */
/******************************************************************************/
static void PeriodicTimerCallback(struct k_timer *pArg);

static MsgTaskArrayEntry_t msgTaskRegistry[FRAMEWORK_MAX_MSG_RECEIVERS];

/******************************************************************************/
/* Global Function Definitions                                                */
/******************************************************************************/
void Framework_Initialize(void)
{
	memset(msgTaskRegistry, 0, sizeof(msgTaskRegistry));

	BufferPool_Initialize();
}

void Framework_RegisterReceiver(FwkMsgReceiver_t *pRxer)
{
	if (pRxer->id >= FRAMEWORK_MAX_MSG_RECEIVERS) {
		return;
	}

	int key = irq_lock();
	{
		/* Waste some memory (ids are constant)
		 * so that a for loop isn't required to look up msg task in table. */
		if (!msgTaskRegistry[pRxer->id].inUse) {
			msgTaskRegistry[pRxer->id].inUse = true;
			msgTaskRegistry[pRxer->id].pMsgReceiver = pRxer;
		} else {
			FRAMEWORK_ASSERT(FORCED);
		}
	}
	irq_unlock(key);
}

void Framework_RegisterTask(FwkMsgTask_t *pMsgTask)
{
	Framework_RegisterReceiver(&pMsgTask->rxer);
	k_timer_init(&pMsgTask->timer, PeriodicTimerCallback, NULL);
}

BaseType_t Framework_Send(FwkId_t RxId, FwkMsg_t *pMsg)
{
	FRAMEWORK_ASSERT(pMsg != NULL);
	BaseType_t result = FWK_ERROR;
	FwkMsgReceiver_t *pMsgRxer = msgTaskRegistry[RxId].pMsgReceiver;
	if (pMsgRxer != NULL) {
		pMsg->header.rxId = RxId;
		result = Framework_Queue(pMsgRxer->pQueue, &pMsg, K_NO_WAIT);
	}
	return result;
}

BaseType_t Framework_Unicast(FwkMsg_t *pMsg)
{
	FRAMEWORK_ASSERT(pMsg != NULL);

	BaseType_t result = FWK_ERROR;
	u32_t i;
	for (i = 0; i < FRAMEWORK_MAX_MSG_RECEIVERS; i++) {
		FwkMsgReceiver_t *pMsgRxer = msgTaskRegistry[i].pMsgReceiver;

		if (pMsgRxer != NULL) {
			/* The handler isn't called here.
			 * It is only used to find the task the message belongs to. */
			FwkMsgHandler_t msgHandler =
				pMsgRxer->pMsgDispatcher(pMsg->header.msgCode);

			/* If there is a dispatcher, then send the message to that task. */
			if (msgHandler != NULL) {
				pMsg->header.rxId = pMsgRxer->id;
				result = Framework_Queue(pMsgRxer->pQueue,
							 &pMsg, K_NO_WAIT);
				break;
			}
		}
	}

	return result;
}

int Framework_Broadcast(FwkMsg_t *pMsg, size_t MsgSize)
{
	FRAMEWORK_ASSERT(pMsg != NULL);
#if FWK_ALLOW_BROADCAST_FROM_ISR
	/* It is technically possible to broadcast from ISR, but isn't recommended. */
	FRAMEWORK_ASSERT(!Framework_InterruptContext());
#endif

	BaseType_t result = FWK_ERROR;

	u32_t i;
	for (i = 0; i < FRAMEWORK_MAX_MSG_RECEIVERS; i++) {
		FwkMsgReceiver_t *pMsgRxer = msgTaskRegistry[i].pMsgReceiver;

		if (pMsgRxer != NULL && pMsgRxer->pMsgDispatcher != NULL) {
			/* The handler isn't called here.  It is only used to determine
			 * if a task should receive a broadcast message. */
			FwkMsgHandler_t msgHandler =
				pMsgRxer->pMsgDispatcher(pMsg->header.msgCode);

			/* If there is a dispatcher,
			 * then create a copy of the message and place on the queue. */
			if (msgHandler != NULL) {
				FwkMsg_t *pNewMsg =
					(FwkMsg_t *)BufferPool_Take(MsgSize);
				if (pNewMsg != NULL) {
					memcpy(pNewMsg, pMsg, MsgSize);
					pNewMsg->header.rxId = pMsgRxer->id;
					result = Framework_Queue(
						pMsgRxer->pQueue, &pNewMsg,
						K_NO_WAIT);

					if (result != FWK_SUCCESS) {
						BufferPool_Free(pNewMsg);
					}
				}
			}
		}
	}

	/* Free Original Message Memory only when all messages were routed.
	 * This conditional is here because the message free should occur in
	 * application code when the result returned is FWK_ERROR.
	 */
	if (result == FWK_SUCCESS) {
		BufferPool_Free(pMsg);
	}

	return result;
}

BaseType_t Framework_Queue(FwkQueue_t *pQueue, void *ppData,
			   TickType_t BlockTicks)
{
	FRAMEWORK_ASSERT(pQueue != NULL);
	FRAMEWORK_ASSERT(ppData != NULL);
	FRAMEWORK_ASSERT(((FwkMsg_t *)ppData)->header.msgCode != FMC_INVALID);
	if (Framework_InterruptContext()) {
		return k_msgq_put(pQueue, ppData, K_NO_WAIT);
	} else {
		return k_msgq_put(pQueue, ppData, BlockTicks);
	}
}

BaseType_t Framework_Receive(FwkQueue_t *pQueue, void *ppData,
			     TickType_t BlockTicks)
{
	FRAMEWORK_ASSERT(pQueue != NULL);
	FRAMEWORK_ASSERT(ppData != NULL);
	if (Framework_InterruptContext()) {
		return k_msgq_get(pQueue, ppData, K_NO_WAIT);
	} else {
		return k_msgq_get(pQueue, ppData, BlockTicks);
	}
}

void Framework_StartTimer(FwkMsgTask_t *pMsgTask)
{
	FRAMEWORK_ASSERT(pMsgTask != NULL);
	k_timer_start(&pMsgTask->timer, pMsgTask->timerDurationTicks,
		      pMsgTask->timerPeriodTicks);
}

void Framework_StopTimer(FwkMsgTask_t *pMsgTask)
{
	FRAMEWORK_ASSERT(pMsgTask != NULL);
	k_timer_stop(&pMsgTask->timer);
}

void Framework_ChangeTimerPeriod(FwkMsgTask_t *pMsgTask, TickType_t Duration,
				 TickType_t Period)
{
	FRAMEWORK_ASSERT(pMsgTask != NULL);
	pMsgTask->timerDurationTicks = Duration;
	pMsgTask->timerPeriodTicks = Period;
	k_timer_start(&pMsgTask->timer, pMsgTask->timerDurationTicks,
		      pMsgTask->timerPeriodTicks);
}

void Framework_MsgReceiver(FwkMsgReceiver_t *pRxer)
{
	FRAMEWORK_ASSERT(pRxer != NULL);

	FwkMsg_t *pMsg = NULL;
	BaseType_t status =
		Framework_Receive(pRxer->pQueue, &pMsg, pRxer->rxBlockTicks);

	if ((status == FWK_SUCCESS) && (pMsg != NULL)) {
		FwkMsgHandler_t msgHandler =
			pRxer->pMsgDispatcher(pMsg->header.msgCode);
		if (msgHandler != NULL) {
			DispatchResult_t result = msgHandler(pRxer, pMsg);
			if (result != DISPATCH_DO_NOT_FREE) {
				BufferPool_Free(pMsg);
			}
		} else {
			Framework_UnknownMsgHandler(pRxer, pMsg);
		}
	}
}

BaseType_t Framework_QueueIsEmpty(FwkId_t RxId)
{
	if (RxId >= FRAMEWORK_MAX_MSG_RECEIVERS) {
		return 1;
	}

	if (!msgTaskRegistry[RxId].inUse) {
		return 1;
	}

	FwkQueue_t *pQueue = msgTaskRegistry[RxId].pMsgReceiver->pQueue;
	return ((k_msgq_num_used_get(pQueue) == 0) ? 1 : 0);
}

/******************************************************************************/
/* Local Function Definitions                                                 */
/******************************************************************************/
static void PeriodicTimerCallback(struct k_timer *pArg)
{
	/* This assumes that the task was created by providing
	 * the pointer to the msg task structure
	 * (or that it is first item in the object).
	 */
	FwkMsgTask_t *pMsgTask =
		(FwkMsgTask_t *)CONTAINER_OF(pArg, FwkMsgTask_t, timer);

	FwkMsg_t *pMsg = (FwkMsg_t *)BufferPool_Take(sizeof(FwkMsg_t));
	if (pMsg != NULL) {
		FRAMEWORK_MSG_HEADER_INIT(pMsg, FMC_PERIODIC,
					  pMsgTask->rxer.id);
		FRAMEWORK_MSG_SEND_TO(pMsgTask->rxer.id, pMsg);
	}
}
