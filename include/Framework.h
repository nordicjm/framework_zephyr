/**
 * @file Framework.h
 * @brief Message Framework for intertask communication.
 *
 * Copyright (c) 2020-2022 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <kernel.h>

/******************************************************************************/
/* Readability                                                                */
/******************************************************************************/
#define EXTERNED
#define PROTOTYPE_FOR_EXTERNED

/******************************************************************************/
/* Prevent unused x warnings                                                  */
/******************************************************************************/
#define UNUSED_VARIABLE(X) ((void)(X))
#define UNUSED_PARAMETER(X) UNUSED_VARIABLE(X)
#define UNUSED_RETURN_VALUE(X) UNUSED_VARIABLE(X)

/******************************************************************************/
/* Common                                                                     */
/******************************************************************************/
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef NUL
#define NUL ('\0')
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#endif

/******************************************************************************/
/* Global Constants, Macros and Type Definitions                              */
/******************************************************************************/

typedef uint8_t FwkMsgCode_t;
typedef uint8_t FwkId_t;

/* Zephyr kernel queue functions use int and return 0 for success */
#define BaseType_t int

/* Zephyr kernel time base is ms, @ref K_NO_WAIT, @ref K_FOREVER */
#define TickType_t k_timeout_t

/* Framework Message Codes (FMC)
 * Application specific codes are defined in config/FrameworkMsgCodes.h.
 */
/* clang-format off */
#define FMC_INVALID                    0
#define FMC_PERIODIC                   1
#define FMC_SOFTWARE_RESET             2
#define FMC_WATCHDOG_CHALLENGE         3
#define FMC_WATCHDOG_RESPONSE          4
#define FMC_ATTR_CHANGED               5
#define FMC_FACTORY_RESET              6
#define FMC_APPLICATION_SPECIFIC_START 8
/* clang-format on */

enum FwkStatusEnum {
	FWK_SUCCESS = 0,
	FWK_ERROR,
};

enum FwkMsgOptionBitmask {
	FWK_MSG_OPTION_NONE = 0,
	/* Callback option requires the callback message type to be used */
	FWK_MSG_OPTION_CALLBACK = BIT(0),
};

typedef enum DispatchResultEnum {
	DISPATCH_OK = 0,
	DISPATCH_ERROR,
	DISPATCH_DO_NOT_FREE,
} DispatchResult_t;

typedef struct FwkMsgHeader {
	FwkMsgCode_t msgCode;
	FwkId_t rxId;
	FwkId_t txId;
	uint8_t options;
} FwkMsgHeader_t;
BUILD_ASSERT(sizeof(FwkMsgHeader_t) == 4, "Unexpected Header Size");

typedef struct FwkMsg {
	FwkMsgHeader_t header;
	/* Optional Payload */
} FwkMsg_t;

/* A generic framework message with a buffer */
typedef struct FwkBufMsg {
	FwkMsgHeader_t header;
	size_t size; /** number of bytes allocated for buffer */
	size_t length; /** number of used bytes in buffer */
	uint8_t buffer[]; /** size is determined by allocator */
} FwkBufMsg_t;

/* Framework callback message
 *
 * Callback occurs in msg receiver context.  Receiver may
 * not know about callback.
 *
 * Can be used to give a semaphore or set an event.
 */
typedef struct FwkCallbackMsg {
	FwkMsgHeader_t header;
	void (*callback)(uint32_t data);
	uint32_t data;
} FwkCallbackMsg_t;

/*
 * Each message task has a message handler or dispatcher.
 * The dispatcher should be implemented using a case statement
 * so that the number of messages doesn't
 * affect the time to determine what handler to call.
 */
typedef struct FwkMsgReceiver FwkMsgReceiver_t;
typedef DispatchResult_t FwkMsgHandler_t(FwkMsgReceiver_t *pMsgRxer,
					 FwkMsg_t *pMsg);
/*
 * Message Framework Receiver Object
 *
 * The dispatcher determines what function should handle each message.
 */
typedef struct k_msgq FwkQueue_t;

struct FwkMsgReceiver {
	FwkId_t id;
	FwkQueue_t *pQueue;
	TickType_t rxBlockTicks;
	FwkMsgHandler_t *(*pMsgDispatcher)(FwkMsgCode_t msgCode);
};

/**
 * @brief Message Framework Task Object
 *
 * Contains a receiver object, task/thread information, and a timer
 */
typedef struct FwkMsgTask {
	FwkMsgReceiver_t rxer;
	struct k_thread threadData;
	struct k_thread *pTid;
	struct k_timer timer;
	TickType_t timerDurationTicks; /* Initial time */
	TickType_t timerPeriodTicks; /* Second time (0 for one shot) */
} FwkMsgTask_t;

/**
 * @brief Get pointer to object containing task (in dispatcher context).
 *
 * Example:
 * DispatchResult_t MsgHandler(FwkMsgReceiver_t *pMsgRxer, FwkMsg_t *pMsg)
 * {
 *   TaskObj_t *pObj = FWK_TASK_CONTAINER(TaskObj_t);
 * }
 */
#define FWK_TASK_CONTAINER(_obj_)                                              \
	CONTAINER_OF(CONTAINER_OF(pMsgRxer, FwkMsgTask_t, rxer), _obj_, msgTask)

#define FWK_BUFFER_MSG_SIZE(t, s) (sizeof(t) + (s))

/* Most framework messages are small.  This is a check that at least
 * one message can be allocated in addition to small messages.
 */
#define CHECK_BUFFER_SIZE(x)                                                   \
	BUILD_ASSERT(x <= (CONFIG_BUFFER_POOL_SIZE / 2),                       \
		     "Buffer Pool is too small")

#define CHECK_FWK_MSG_SIZE(m) CHECK_BUFFER_SIZE(sizeof(m))

#define FWK_QUEUE_ENTRY_SIZE (sizeof(FwkMsg_t *))
#define FWK_QUEUE_ALIGNMENT 4 /* bytes */

/* Routing a message to task id 0 indicates a problem */
#define FWK_ID_RESERVED 0
#define FWK_ID_APP_START 1

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/
/**
 * @brief This function registers a Message Task or Receiver with the
 * Message Framework.  This is necessary to support routing of
 * messages. A critical section is entered when a task is
 * registered.  However, the task should be registered before the
 * task is created so that the periodic timer can be created before
 * the task starts.
 *
 * @param pMsgRxer A message receiver.
 * @param pMsgTask Pointer to a Message Task (which contains a rxer).
 *
 * Messages can be routed based on their ID or based on whether or not
 * they have a function mapped to a message code in their dispatcher.
 *
 * @ref FwkTaskIds.h
 */
void Framework_RegisterReceiver(FwkMsgReceiver_t *pRxer);
void Framework_RegisterTask(FwkMsgTask_t *pMsgTask);

/**
 * @brief Wraps the queue receive function of the OS and waits for
 * rxBlockTicks for a message to arrive in a task's queue.
 * When a message is received the appropriate message handler
 * function is called by the dispatcher.
 */
void Framework_MsgReceiver(FwkMsgReceiver_t *pMsgRxer);

/**
 * @brief Sends a message to a single task based on a task ID.
 *
 * @retval An assert isn't generated if RxId is invalid.
 * @note Caller is responsible for freeing memory, if status isn't success.
 */
BaseType_t Framework_Send(FwkId_t RxId, FwkMsg_t *pMsg);

/**
 * @brief Sends a single message to a single task by searching the
 * dispatcher of each message receiver.
 *
 * @note Prevents indirect coupling of tasks by message IDs.
 * @note This should not be used for messages that can go to more than
 * one destination.
 * @note As the number of tasks increases, the amount of time to find the
 * rxID increases.
 *
 * @retval Caller is responsible for freeing memory, if status isn't success.
 */
BaseType_t Framework_Unicast(FwkMsg_t *pMsg);

/**
 * @brief Copies a message and sends it to all tasks that have the message
 * code in their dispatcher.
 *
 * @param MsgSize required to copy message.
 *
 * @note Currently an assertion fires if this is called in interrupt context.
 *
 * @retval Caller is responsible for freeing memory, if status isn't success.
 */
BaseType_t Framework_Broadcast(FwkMsg_t *pMsg, size_t MsgSize);

/**
 * @brief Bypasses message router and puts a message directly on a queue.
 *
 * @note Most commonly used by a task to send a message to itself.
 */
BaseType_t Framework_Queue(FwkQueue_t *pQueue, void *ppData,
			   TickType_t BlockTicks);

/**
 * @retval 0 if not not empty
 */
BaseType_t Framework_QueueIsEmpty(FwkId_t RxId);

/**
 * @brief Free all messages in a receiver's queue.
 *
 * @retval Number of messages that were purged.
 */
size_t Framework_Flush(FwkId_t RxId);

/**
 * @brief Blocks on queue waiting for a message.
 *
 * @note If the caller checks that *ppData is not NULL
 * (without checking return), then it must initialize *ppData.
 * Example:
 * FwkMsg_t *pMsg = NULL;
 * Framework_Receive(q, &pMsg, 0);
 * if (pMsg != NULL) { ...
 *
 * @note Only needed in special cases.
 * This function is called by Framework_MsgReceiver.
 */
BaseType_t Framework_Receive(FwkQueue_t *pQueue, void *ppData,
			     TickType_t BlockTicks);
/**
 * @brief Starts a task's periodic timer
 */
void Framework_StartTimer(FwkMsgTask_t *pMsgTask);

/**
 * @brief Stops a task's periodic timer.
 */
void Framework_StopTimer(FwkMsgTask_t *pMsgTask);

/**
 *@brief Changes the period of a task's periodic timer.
 */
void Framework_ChangeTimerPeriod(FwkMsgTask_t *pMsgTask, TickType_t Duration,
				 TickType_t Period);

/******************************************************************************/
/* Define in application or weak implementation will be used.                 */
/******************************************************************************/
extern void Framework_AssertionHandler(char *file, int line);
extern bool Framework_InterruptContext(void);
extern void Framework_SystemReset(void);
extern DispatchResult_t Framework_UnknownMsgHandler(FwkMsgReceiver_t *pMsgRxer,
						    FwkMsg_t *pMsg);

/******************************************************************************/
/* Framework Assertions                                                       */
/******************************************************************************/
/* clang-format off */
#if CONFIG_FWK_ASSERT_ENABLED_USE_ZEPHYR
	#define FRAMEWORK_ASSERT(expr) __ASSERT(expr, "Framework Assertion")
#elif CONFIG_FWK_ASSERT_ENABLED
	/* Shortened file names make it easier to support assertions that
	 * print the file name on processors with limited flash space. */
	#ifndef FNAME
		#ifndef FWK_FNAME
			#define FNAME __FILE__
		#else
			#define FNAME FWK_FNAME
		#endif
	#endif

	#define FRAMEWORK_ASSERT(expr) do { \
		if( !(expr) ) { Framework_AssertionHandler(FNAME, __LINE__); } \
	}while(0)
#else
	#define FRAMEWORK_ASSERT(expr)
#endif

/* Use a debug assert if you don't want the assert to reset the processor.
 * This is for backward compatibility.
 * Zephyr's __ASSERT_EVAL could be used instead.
 */
#if CONFIG_FWK_DEBUG
	#define FRAMEWORK_DEBUG_ASSERT(expr) FRAMEWORK_ASSERT(expr)
#else
	#define FRAMEWORK_DEBUG_ASSERT(expr)
#endif

/* Force an assertion to fire */
#define FORCED 0

/* clang-format on */

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_H__ */
