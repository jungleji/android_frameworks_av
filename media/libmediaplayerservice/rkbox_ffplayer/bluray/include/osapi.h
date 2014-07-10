/*****************************************************************************
******************************************************************************
**                                                                          **
**  Copyright (c) 2002 Videon Central, Inc.                                 **
**  All rights reserved.                                                    **
**                                                                          **
**  The computer program contained herein contains proprietary information  **
**  which is the property of Videon Central, Inc.  The program may be used  **
**  and/or copied only with the written permission of Videon Central, Inc.  **
**  or in accordance with the terms and conditions stipulated in the        **
**  agreement/contract under which the programs have been supplied.         **
**                                                                          **
******************************************************************************
*****************************************************************************/
/**
 * @file osapi.h
 *
 * Operating System API (OSAPI) header file. Provides an abstraction from the
 * operating system
 *
 * $Id: osapi.h,v 1.18 2006/10/25 13:08:09 mspinnenweber Exp $
 */
#ifndef OSAPI_H
#define OSAPI_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Header files                                                             **
**                                                                           **
*******************************************************************************
******************************************************************************/
#include "vcitypes.h"


/******************************************************************************
*******************************************************************************
**                                                                           **
**  OSAPI defines                                                            **
**                                                                           **
*******************************************************************************
******************************************************************************/

/*
 *  General
 */
#define OS_TIMEOUT              2       /* JCA - ADDED */
#define OS_MBOX_FULL            1       /* JCA - ADDED */

/*
 *  Error codes
 */
#define OS_OK                   0
#define OS_FAILURE              (-1)
#define OS_NULL_POINTER         (-2)
#define OS_NOT_IMPLEMENTED      (-3)
#define OS_NO_MEMORY            (-4)
#define OS_INVALID_MEMPOOL      (-5)

/*
 * Time Delays
 */
#define OS_WAIT_FOREVER         (-1)
#define OS_NO_WAIT              0       /* JCA - ADDED */
#define OS_WAIT_1S              (OS_GetTickRate())
#define OS_WAIT_2S              (OS_GetTickRate() << 1)
#define OS_WAIT_4S              (OS_GetTickRate() << 2)
#define OS_WAIT_8S              (OS_GetTickRate() << 3)
#define OS_WAIT_16S             (OS_GetTickRate() << 4)

/*
 *  Tasks
 */
#define OS_TASK_MIN_PRIORITY     1       /* rbehe - added */
#define OS_TASK_MAX_PRIORITY     256     /* rbehe - added */
#define OS_TASK_LOW_PRIORITY     64
#define OS_TASK_NORMAL_PRIORITY  128
#define OS_TASK_RAISED_PRIORITY  160
#define OS_TASK_HIGH_PRIORITY    192

/* queue depth and priorities */
#define IG_PARSE_PRIORITY               OS_TASK_NORMAL_PRIORITY
#define IG_RENDER_PRIORITY              OS_TASK_HIGH_PRIORITY  // high??
#define IG_PARSE_STACK_SIZE             (16*1024)
#define IG_RENDER_STACK_SIZE            (16*1024)
#define IG_PARSE_Q_DEPTH                5000  /* YES, we NEED this many, section 8.8.2.2 june 30 amendment to bluray 2.0 */
#define IG_RENDER_Q_DEPTH               30


/*
 *  Message queues
 */
#define OS_MSG_Q_FIFO           0x00    /* Used with message queues */
#define OS_MSG_PRI_NORMAL	    0
#define OS_MSG_PRI_URGENT	    1

/*
 *  Semaphores
 */
#define OS_SEM_Q_FIFO           0x00
#define OS_SEM_Q_PRIORITY       0x01
#define OS_SEM_EMPTY            0       /* For OS_SEM_B_STATE type */
#define OS_SEM_FULL             1
#define OS_SEM_BINARY           0       /* JCA - ADDED */
#define OS_SEM_COUNTING         1       /* JCA - ADDED */
#define OS_SEM_NULL             0       /* JMU - ADDED */

/*
 *  Miscellaneous
 */
#define MSB(x) (((x) >> 8) & 0xff)  /* Used only in vcd_fifo.c */
#define LSB(x) ((x) & 0xff)

/******************************************************************************
*******************************************************************************
**                                                                           **
**  OSAPI typedefs                                                           **
**                                                                           **
*******************************************************************************
******************************************************************************/
/*
 *  General
 */
typedef int OS_STATUS;              /* prefix: none  */


/*
 *  Tasks
 */
typedef ULONG (*OS_FUNCPTR)(void);          /* prefix: func  */
typedef ULONG (*OS_FUNCPTR_PARAM)(PVOID);   /* prefix: func  */

/* Message Queue Handle */
typedef ULONG OS_MSG_Q_ID;

/* Semaphores Handle */
typedef ULONG OS_SEM_ID;
typedef BOOLEAN OS_SEM_B_STATE;

/* Memory Handle */
typedef PVOID OS_MEMPOOL_ID;

/* Timers Handle */
typedef ULONG OS_TIMER_ID;

/*
 *  Devices
 */
typedef struct                      /* prefix: node  */
{
	struct os_node *next;
	struct os_node *previous;
} OS_NODE;

typedef struct                      /* prefix: dev   */
{
    OS_NODE	node;
    SHORT	drvNum;
    char    *name;
} OS_DEV_HDR;

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Task function prototypes                                                 **
**                                                                           **
*******************************************************************************
******************************************************************************/
OS_STATUS OS_TaskSpawn(char *strName, int iPrio, int iStackSize, OS_FUNCPTR func);
OS_STATUS OS_TaskSpawnParam(char *strName, int iPrio, int iStackSize, OS_FUNCPTR_PARAM func,
                            PVOID pvParam, PVOID pvStackBaseParam);
void OS_TaskDelete(int taskId);
void OS_TaskExit(void);
OS_STATUS OS_TaskReset(int taskId);
OS_STATUS OS_TaskSuspend(int taskId);
OS_STATUS OS_TaskResume(int taskId);
#define OS_TaskRestart OS_TaskResume                  /**< for legacy code */
void OS_TaskYield(void);
void OS_TaskDelay(int iTicks);
void OS_TaskDelayMsec(int milliseconds);
OS_STATUS OS_TaskPrioritySet(int taskId, int iPrio);
ULONG OS_TaskIdSelf(void);
BOOLEAN OS_TaskLock(void);
BOOLEAN OS_TaskUnlock(void);
OS_STATUS OS_TaskJoin(int taskId);
OS_STATUS OS_TaskVerify(int taskId);
int OS_TaskNameToId(char *strName);
char *OS_TaskName(int taskId);
void OS_TaskShowPc(int taskId);

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Message queue function prototypes                                        **
**                                                                           **
*******************************************************************************
******************************************************************************/
OS_MSG_Q_ID OS_MsgQCreate(int iDepth, int iMaxMsgSize, int iOSOptions);
OS_STATUS OS_MsgQDelete(OS_MSG_Q_ID mqId);
OS_STATUS OS_MsgQSend(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize, int iTimeout, int iPrio);

//#define NDEBUG

//#if defined(NDEBUG)
extern OS_STATUS OS_MsgQReceive(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize, int iTimeout);
OS_STATUS OS_MsgQRemove(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize);
/*
#else
#define  OS_MsgQReceive(mqId, strMsg, ulSize, iTimeout)  OS_MsgQReceiveDbg(mqId, strMsg, ulSize, iTimeout, __FILE__, __LINE__)
OS_STATUS OS_MsgQReceiveDbg(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize, int iTimeout, char *srcFile, int srcLine);
#define OS_MsgQRemove(mqId, strMsg, ulSize)  OS_MsgQRemoveDbg(mqId, strMsg, ulSize, __FILE__, __LINE__)
OS_STATUS OS_MsgQRemoveDbg(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize, char *srcFile, int srcLine);
#endif
*/
OS_STATUS OS_MsgQReset(OS_MSG_Q_ID mqId);
int OS_MsgQNumMsgs(OS_MSG_Q_ID mqId);

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Semaphore function prototypes                                            **
**                                                                           **
*******************************************************************************
******************************************************************************/
OS_SEM_ID OS_SemBCreate(int iOSOptions, OS_SEM_B_STATE semsInitialState);
OS_SEM_ID OS_SemBCreateNamed(int iOSOptions, OS_SEM_B_STATE semsInitialState, char *strName);
OS_SEM_ID OS_SemCCreate(int iOSOptions, int iInitialCount);
OS_SEM_ID OS_SemCCreateNamed(int iOSOptions, int iInitialCount, char *strName);

#if defined(NDEBUG)
OS_STATUS OS_SemTake(OS_SEM_ID semId, int iTimeout);
OS_STATUS OS_SemTakeMsec(OS_SEM_ID semId, int milliseconds);
OS_STATUS OS_SemGive(OS_SEM_ID semId);
OS_STATUS OS_SemDelete(OS_SEM_ID semId);
#else

#define OS_SemTake(semId, iTimeout)  OS_SemTakeDbg((semId), (iTimeout), __FILE__, __LINE__)
OS_STATUS OS_SemTakeDbg(OS_SEM_ID semId, int iTimeout, char *srcFile, int srcLine);

#define OS_SemTakeMsec(semId, iTimeout)  OS_SemTakeMsecDbg((semId), (iTimeout), __FILE__, __LINE__)
OS_STATUS OS_SemTakeMsecDbg(OS_SEM_ID semId, int milliseconds, char* srcFile, int srcLine);

#define OS_SemGive(semId)  OS_SemGiveDbg((semId), __FILE__, __LINE__)
OS_STATUS OS_SemGiveDbg(OS_SEM_ID semId, char *srcFile, int srcLine);

#define OS_SemDelete(semId) OS_SemDeleteDbg((semId), __FILE__, __LINE__)
OS_STATUS OS_SemDeleteDbg(OS_SEM_ID semId, char *srcFile, int srcLine);

#endif

OS_STATUS OS_SemFlush(OS_SEM_ID semId);

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Memory function prototypes                                               **
**                                                                           **
*******************************************************************************
******************************************************************************/
void OS_MemoryInfo(OS_MEMPOOL_ID pool);
OS_STATUS OS_InitDramTop(PVOID pStartAddr, ULONG ulMemSize);
#ifndef DMALLOC
PVOID OS_MemAlloc(ULONG ulSize);
OS_STATUS OS_MemFree(PVOID pvMemory);
#endif
OS_MEMPOOL_ID OS_CreateMemPool(PVOID pAddr, ULONG ulSize, ULONG ulPageSize);
OS_STATUS OS_DeleteMemPool(OS_MEMPOOL_ID pool);
PVOID OS_MemPoolAlloc(OS_MEMPOOL_ID pool, ULONG ulSize);
OS_STATUS OS_MemPoolFree(OS_MEMPOOL_ID pool, PVOID pvMemory);
OS_STATUS OS_MemPoolReset(OS_MEMPOOL_ID pool);

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Timer function prototypes                                                **
**                                                                           **
*******************************************************************************
******************************************************************************/
OS_STATUS  OS_TimerCreate(OS_TIMER_ID *ptimerId, OS_FUNCPTR func);
OS_STATUS  OS_TimerCreateParam(OS_TIMER_ID *ptimerId, OS_FUNCPTR_PARAM func, PVOID pvParam);
OS_STATUS  OS_TimerDelete(OS_TIMER_ID ptimerId);
OS_STATUS  OS_TimerStop(OS_TIMER_ID ptimerId);
OS_STATUS  OS_TimerSet(OS_TIMER_ID ptimerId, ULONG ulPeriod);
OS_STATUS  OS_TimerSetMsec(OS_TIMER_ID ptimerId, ULONG ulPeriod);
OS_STATUS  OS_TimerSetRepeat(OS_TIMER_ID ptimerId, ULONG ulPeriod);
OS_STATUS  OS_TimerSetRepeatMsec(OS_TIMER_ID ptimerId, ULONG ulPeriod);
OS_STATUS  OS_TimerGetTime(OS_TIMER_ID ptimerID, ULONG *pulTimeRemaining);
OS_STATUS  OS_TimerGetTimeMsec(OS_TIMER_ID ptimerID, ULONG *pulTimeRemaining);
OS_FUNCPTR GetTimerFunc(OS_TIMER_ID ptimerId);
ULONG      OS_GetTickRate(void);
ULONG      OS_GetTicks(void);
#define OS_GetTicksPerSecond OS_GetTickRate    /**< for legacy code */
#define OS_GetTickCount      OS_GetTicks       /**< for legacy code */

/******************************************************************************
*******************************************************************************
**                                                                           **
**  I/O function prototypes                                                  **
**                                                                           **
*******************************************************************************
******************************************************************************/
OS_STATUS OS_Open(char *strPath, int iOFlags);
void      OS_Close(ULONG ulFileDes);
OS_STATUS OS_iosDevAdd(OS_DEV_HDR *pdevHdr, char *name, int iDrvnum);
OS_STATUS OS_iosDrvInstall( OS_FUNCPTR funcCreate,
                            OS_FUNCPTR funcDelete,
                            OS_FUNCPTR funcOpen,
                            OS_FUNCPTR funcClose,
                            OS_FUNCPTR funcRead,
                            OS_FUNCPTR funcWrite,
                            OS_FUNCPTR funcIOCTL );
OS_STATUS OS_IntLock(void);
void      OS_IntUnlock(int iLockKey);

#ifdef __cplusplus
}
#endif

#endif

