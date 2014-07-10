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
 * @file osapi_linux.cpp
 *
 * $Revision: 1.63 $ 
 *
 * Operating System API (OSAPI) source file. Provides an abstraction from the
 * operating system.
 *
 * This is the implementation for Linux
 *
 */
#define POSIX_SEMAPHORE
//#include "osapi_linux.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>

//#include<pthread.h>

//#include "dbgprint.h"


//#define __CALC_PEAK_ALLOCATION


#include "include/LOGD.h"


#define LOG_TAG "OSAPI"

#include <pthread.h>
#include "include/vdvd_types.h"
#include "include/osapi.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif



//using namespace RKBluray;

#define MAX_NAME_SIZE 16
#define OS_MSG_Q_HEAD 0
#define OS_MSG_Q_TAIL 1

/* TYPEDEFS */

#ifdef __CALC_PEAK_ALLOCATION
typedef struct tag_mem_info
{
    PVOID pvAddr;
    ULONG size;
} mem_info;
#endif

/*
 *  Thread record
 */
typedef struct tagThreadRecord              /* prefix: thread */
{
    PVOID           pvFunction;
    PVOID           pvParam;
    PVOID           pvStackBase;
    pthread_t       pthread;
    pthread_attr_t  attr;
} OS_TASK;

/*
 *  Semaphore record
 */
typedef struct tagSemaphoreRecord           /* prefix: sem */
{
    ULONG           ulSemType;
    char            strName[MAX_NAME_SIZE];
    ULONG           ulCount;
    ULONG           ulMaxCount;
    pthread_cond_t  condWait;       /* Signal waiting threads */
    pthread_mutex_t mutex;          /* Critical sections */

} OS_SEMAPHORE;

/*
 *  Message Box record
 */
typedef struct tagMessageBoxRecord          /* prefix: mbox */
{
    ULONG           ulDepth;                /* message queue depth */
    ULONG           ulSize;                 /* message size */
    ULONG           ulHead;
    ULONG           ulTail;
    BYTE           *pbBuffers;              /* buffer for messages */
    pthread_cond_t  condWaitFull;           /* Signal waiting threads */
    pthread_cond_t  condWaitEmpty;          /* Signal waiting threads */
    pthread_mutex_t mutex;                  /* Critical sections */
} OS_MBOX;

typedef struct tagTimerRecord
{
    PVOID           pvFunction;
    PVOID           pvParam;
    timer_t         timerID;
} OS_TIMER;


/*
 * Memory Pool ID record
 */
typedef struct tagMemPoolRecord
{
    ULONG          ulPgSize;           /* Size of each page of memory */
    ULONG          ulNumPages;         /* Number of memory pages */
    struct Page                        /* A page from a memory pool */
    {
        PVOID      pvPgAddr;           /* Pointer to memory address */
        LONG       lAttached;          /* Number of pages attached with page *
                                        * (including itself)                 */
    } *memPage;
} OS_MEMPOOL;

#ifdef __CALC_PEAK_ALLOCATION
mem_info test[512];
BOOLEAN fFirstAlloc          = TRUE;
ULONG   ulCurrentAllocations = 0;
ULONG   ulPeakAllocations    = 0;
#endif

/* private function prototypes */
static PVOID linuxThread(PVOID);
static void linuxTimer(union sigval sigval);

#ifdef VC_TIMERS
int vc_timer_delete(timer_t timerid);
int vc_timer_create(clockid_t clock_id, struct sigevent *evp, timer_t *timerid);
int vc_timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue);
int vc_timer_gettime(timer_t timerid, struct itimerspec *value);
static ULONG TimerThreadProc( PVOID pvParam );

#define timer_delete  vc_timer_delete
#define timer_create  vc_timer_create
#define timer_settime vc_timer_settime
#define timer_gettime vc_timer_gettime
#endif

#ifdef __CALC_PEAK_ALLOCATION
mem_info test[512];
BOOLEAN fFirstAlloc          = TRUE;
ULONG   ulCurrentAllocations = 0;
ULONG   ulPeakAllocations    = 0;
#endif

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;


/* private function prototypes */
static PVOID linuxThread(PVOID);
static void linuxTimer(union sigval sigval);

#ifdef VC_TIMERS
int vc_timer_delete(timer_t timerid);
int vc_timer_create(clockid_t clock_id, struct sigevent *evp, timer_t *timerid);
int vc_timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue);
int vc_timer_gettime(timer_t timerid, struct itimerspec *value);
static ULONG TimerThreadProc( PVOID pvParam );

#define timer_delete  vc_timer_delete
#define timer_create  vc_timer_create
#define timer_settime vc_timer_settime
#define timer_gettime vc_timer_gettime
#endif



static OS_STATUS OS_MsgQReceiveOption(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize, int iTimeout, int iOption);

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Task function definitions                                                **
**                                                                           **
*******************************************************************************
******************************************************************************/

/**
 * OS Task Spawn function.
 *
 * @param
 *    char *strName - task name
 *
 * @param
 *    int iPrio - task priority
 *
 * @param
 *    int iStackSize - task stack size
 *
 * @param
 *    OS_FUNCPTR func - task procedure function
 *
 * @retval
 *    int pthreadRecord - pointer to the task record, if successful
 *    OS_FAILURE if not successful.
 */
OS_STATUS OS_TaskSpawn(char *strName,
                       int iPrio,
                       int iStackSize,
                       OS_FUNCPTR func
                       )
{
    return (OS_TaskSpawnParam( strName, iPrio, iStackSize, (OS_FUNCPTR_PARAM)func, NULL, NULL ));
}


/**
 * OS Task Spawn Parameter function.
 *
 * @param
 *    char *strName - task name
 *
 * @param
 *    int iPrio - task priority
 *
 * @param
 *    int iStackSize - task stack size
 *
 * @param
 *    OS_FUNCPTR func - task procedure function
 *
 * @param
 *    PVOID pvParam - task input parameter
 *
 * @param
 *    PVOID pvStackBaseParam - stack pointer, allows stack base to be passed
 *                             in rather than OS_Malloc'd.
 *
 * @retval
 *    int pthreadRecord - pointer to the task record, if successful
 *    OS_FAILURE if not successful.
 */
OS_STATUS OS_TaskSpawnParam(char *strName,
                            int iPrio,
                            int iStackSize,
                            OS_FUNCPTR_PARAM func,
                            PVOID pvParam,
                            PVOID pvStackBaseParam
                            )
{
    OS_TASK *pthreadRecord;
    int     iRetStatus      = 0;

    /* Allocate a record for the thread */
    pthreadRecord = (OS_TASK *)OS_MemAlloc(sizeof(OS_TASK));
    if (pthreadRecord == NULL)
    {
        LOGD("OS_TaskSpawn: Could not allocate thread record!\n");
        goto error_handler;
    }

    /* Check the validity of the given priority */
    if ( (iPrio < OS_TASK_MIN_PRIORITY) || (iPrio > OS_TASK_MAX_PRIORITY) )
    {
        LOGD("%s: Invalid thread priority (%d)!\n", __FUNCTION__, iPrio);
        goto error_handler;
    }

    /* Save the paramater */
    pthreadRecord->pvParam = pvParam;

    /* Record the pointer to the thread stack base in the thread record */
    pthreadRecord->pvStackBase = pvStackBaseParam;

    /* Record the function pointer */
    pthreadRecord->pvFunction = (PVOID)func;

    /* Create the thread attributes */
    iRetStatus = pthread_attr_init(&pthreadRecord->attr);
    if (iRetStatus != 0)
    {
        LOGD("%s: ATTRIBUTE CREATION FAILED!\n", __FUNCTION__);
        goto error_handler;
    }

    /* Specify the threads stack size */
    pthread_attr_setstacksize(&pthreadRecord->attr, iStackSize);
    if (iRetStatus != 0)
    {
        /* this is not a critical error so just let it be known and continue
         * setting up the thread */
        LOGD("%s: Could not set our stack size, Using default!\n", __FUNCTION__);
    }


    /* Create the thread */
    iRetStatus = pthread_create(&pthreadRecord->pthread, &pthreadRecord->attr, linuxThread, (PVOID)pthreadRecord);
    if (iRetStatus != 0)
    {
        LOGD("%s: Handle is NULL!\n", __FUNCTION__);
        goto error_handler;
    }
//    LOGD("\nOS_TaskSpawnParam(),name = %s,pthreadRecord->pthread = %d\n",strName,pthreadRecord->pthread);
    /* Return the thread record as an integer */
    return ( (int)pthreadRecord );

error_handler:

    if (NULL != pthreadRecord)
    {
        OS_MemFree(pthreadRecord);
    }

    return (OS_FAILURE);
}


/**
 * OS Task Delete function.
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    This function will print a status message if it was not successful.
 */
void OS_TaskDelete(int taskId)
{
    OS_TASK *pthreadRecord;
    int iRetStatus;

    /* Cast the given task identifier to a pointer to a task record */
    pthreadRecord = (OS_TASK *)taskId;

    if (pthreadRecord != 0)
    {
        /* Delete the given thread */
        if(pthreadRecord->pthread != 0)
        {
            #if 0
            if((iRetStatus = pthread_kill(pthreadRecord->pthread, SIGQUIT)) != 0)    
            {    
                LOGD("Error cancelling thread %d, error = %d (%d)", pthreadRecord->pthread, iRetStatus);   
            }
            #endif
        }
        /* wait for the task to exit */
        OS_TaskJoin(taskId);

        /* delete the pthread attributes */
        iRetStatus = pthread_attr_destroy(&pthreadRecord->attr);

        /* Delete the thread record */
        OS_MemFree(pthreadRecord);
    }
}


/**
 * OS Task Exit function.
 *
 * @param
 *    None.
 *
 * @retval
 *    None.
 */
void OS_TaskExit(void)
{
    pthread_exit(0);
}


/**
 * Restarts the task from the beginning of the function passed into the spawn command.
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This function is always successful.
 */
OS_STATUS OS_TaskReset(int taskId)
{
    return (OS_NOT_IMPLEMENTED);
}


/**
 * OS Task Suspend function.
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This function is always successful.
 */
OS_STATUS OS_TaskSuspend(int taskId)
{
    return (OS_NOT_IMPLEMENTED);
}


/**
 * Resume a task after a suspend. This replaces OS_TaskRestart!
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This function is always successful.
 */
OS_STATUS OS_TaskResume(int taskId)
{
    return (OS_NOT_IMPLEMENTED);
}


/**
 * OS Task Join function.
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This function is always successful.
 */
OS_STATUS OS_TaskJoin(int taskId)
{
    OS_STATUS status = OS_OK;
    OS_TASK   *pthreadRecord;

    /* Cast the given task identifier to a pointer to a task record */
    pthreadRecord = (OS_TASK *)taskId;

    if (pthreadRecord != 0)
    {
        /* Join the given thread */
        if (pthread_join(pthreadRecord->pthread, NULL) != 0)
        {
            status = OS_FAILURE;
        }
    }
    else
    {
        status = OS_FAILURE;
    }

    return (status);
}

/**
 * OS Task Yield function.
 *
 * @param
 *    None.
 *
 * @retval
 *    This function is always successful.
 */
void OS_TaskYield(void)
{
    if (sched_yield() == -1)
    {
        LOGD("failed to yield\n");
    }
}

/**
 * OS Task Delay function.
 *
 * @param
 *    int iTicks - time to delay the current task, in ticks
 *
 * @retval
 *    This function is always successful.
 */
void OS_TaskDelay(int iTicks)
{
#if (CLOCKS_PER_SEC == 1000000)
    usleep(iTicks);
#else
    ULONGLONG microseconds;
    microseconds = (ULONGLONG)iTicks;
    microseconds *= 1000000;
    microseconds /= CLOCKS_PER_SEC;
    usleep( (unsigned long)microseconds );
#endif
}

/**
 * OS Task Delay function.
 *
 * @param
 *    milliseconds - time to delay the current task, in milliseconds
 *
 * @retval
 *    This function is always successful.
 */
void OS_TaskDelayMsec(int milliseconds)
{
    usleep(milliseconds*1000);
}

/**
 * OS Task Priority Set function
 *
 * @param
 *    int taskId - task identifier
 *
 * @param
 *    int iPrio - task priority
 *
 * @retval
 *    OS_OK if successful.
 *    OS_FAILURE if not successful.
 */
OS_STATUS OS_TaskPrioritySet(int taskId, int iPrio)
{
    OS_TASK *pthreadRecord;

    /*
     *  Cast the given task identifier to a pointer to a task record
     */
    pthreadRecord = (OS_TASK *)taskId;

    /*
     *  Make sure the given priority value is valid
     */
    if( (iPrio < 1) || (iPrio > 64) )
    {
        LOGD("OS_TaskPrioritySet: Invalid priority value (%d)!\n", iPrio);
        return OS_FAILURE;
    }

    /*
     *  Set the priority of the given thread
     */
    /* <TBD> */

    /*
     *  Return successfully
     */
    return OS_OK;
}

/**
 * OS Task Identify Self function.
 *
 * @param
 *    None.
 *
 * @retval
 *    int tHandle - handle of current task, if successful
 *    OS_FAILURE - if not successful
 */
ULONG OS_TaskIdSelf(void)
{
    return((ULONG)pthread_self());
}

/**
 * OS Task Lock function.
 *
 * @param
 *    None.
 *
 * @retval
 *    Returns FALSE on success, TRUE on failure.
 */
BOOLEAN OS_TaskLock(void)
{
    if (pthread_mutex_lock(&global_mutex) != 0)
    {
        return (TRUE);
    }

    return (FALSE);
}

/**
 * OS Task Unlock function.
 *
 * @param
 *    None.
 *
 * @retval
 *    Returns FALSE on success, TRUE on failure.
 */
BOOLEAN OS_TaskUnlock(void)
{
    if (pthread_mutex_unlock(&global_mutex) != 0)
    {
        return (TRUE);
    }

    return (FALSE);
}

/**
 * OS Task Name function.
 *
 * @param
 *    OS_TASK taskId - task identifier
 *
 * @retval
 *    char *strTaskName - task name, if successful
 *    NULL - if not successful
 */
char *OS_TaskName(int taskId)
{
    LOGD("OS_TaskName: NOT IMPLEMENTED!\n");
    return (char *)NULL;
}

/**
 * OS Task Verify function.
 *
 * @param
 *    OS_TASK taskId - task identifier
 *
 * @retval
 *    OS_OK if successful.
 *    OS_FAILURE if not successful.
 */
OS_STATUS OS_TaskVerify(OS_TASK taskId)
{
    LOGD("OS_TaskVerify: NOT IMPLEMENTED!\n");
    return OS_FAILURE;
}

/**
 * OS Task Name to Identifier function.
 *
 * @param
 *    char *strName - task name
 *
 * @retval
 *    int taskId - identifier of current task, if successful
 *    0 - if not successful
 */
int OS_TaskNameToId(char *strName)
{
    LOGD("OS_TaskNameToId: NOT IMPLEMENTED!\n");
    return 0;
}

/**
 * OS Task Show PC function.
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    None.
 */
void OS_TaskShowPc(int taskId)
{
    LOGD("OS_TaskShowPc: NOT IMPLEMENTED!\n");
}

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Message queue function definitions                                       **
**                                                                           **
*******************************************************************************
******************************************************************************/

/**
 * OS_MsgQCreate creates a message queue.
 *
 * @param int iDepth     - queue depth
 * @param int iMsgSize   - message size, all messages placed into this queue must be of this size.
 * @param int iOSOptions - operating system options (not used in this implementation)
 *
 * @retval
 *    OS_MSG_Q_ID mqId - pointer to the message box record, if successful
 *    0 - if not successful
 */
OS_MSG_Q_ID OS_MsgQCreate(int iDepth, int iMsgSize, int iOSOptions)
{
    OS_MBOX *pmsg = NULL;

    /* Check the validity of the given message size */
    if (iMsgSize <= 0)
    {
    //    LOGD(("%s: Invalid message size (%d)!\n", __FUNCTION__, iMsgSize));
    //    LOGD("Invalid message size %d\n", iMsgSize);
        goto error_handler;
    }

    /* Check the validity of the given depth */
    if (iDepth <= 0)
    {
  //      LOGD(("%s: Invalid queue depth (%d)!\n", __FUNCTION__, iDepth));
  //      LOGD(("Invalid queue depth (%d)!\n", iDepth));
        goto error_handler;
    }

    /* Allocate Record */
    pmsg = (OS_MBOX *)OS_MemAlloc(sizeof(OS_MBOX));
    if (NULL == pmsg)
    {
        goto error_handler;
    }

    /* Allocate Buffers */
    pmsg->pbBuffers = (BYTE *)OS_MemAlloc(iMsgSize * (iDepth + 1));
    if (NULL == pmsg->pbBuffers)
    {
        goto error_handler;
    }

    /* Setup Head and Tail */
    /* If Head == Tail Buffers are empty */
    pmsg->ulHead  = 0;
    pmsg->ulTail  = 0;
    pmsg->ulDepth = (ULONG)iDepth;
    pmsg->ulSize  = (ULONG)iMsgSize;

    /* Create the binary semaphore */
    if (pthread_cond_init(&pmsg->condWaitEmpty, NULL) != 0)
    {
        LOGD("OS_MsgQCreate: Could not init condWait");
        goto error_handler;
    }
    if (pthread_cond_init(&pmsg->condWaitFull, NULL) != 0)
    {
        LOGD("OS_MsgQCreate: Could not init condWait");
        goto error_handler;
    }
    if (pthread_mutex_init(&pmsg->mutex, NULL) != 0)
    {
        LOGD("OS_MsgQCreate: Could not init mutex");
        goto error_handler;
    }

    return ((OS_MSG_Q_ID)pmsg);

error_handler:
    if (NULL != pmsg)
    {
        if (NULL != pmsg->pbBuffers)
        {
            OS_MemFree(pmsg->pbBuffers);
        }

        OS_MemFree(pmsg);
    }

    return (0);
}

/**
 * OS Message Queue Delete function.
 *
 * @param
 *    OS_MSG_Q_ID mqId - message queue identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This function is always successful.
 */
OS_STATUS OS_MsgQDelete(OS_MSG_Q_ID mqId)
{
    OS_MBOX   *pmsg  = (OS_MBOX *)mqId;

    if (mqId == 0)
    {
   //     LOGD(("%s: BAD POINTER\n", __FUNCTION__));
        return (OS_FAILURE);
    }

    /* take the mutex to protect during cond destroy and to make sure noone else is holding the mutex */
    pthread_mutex_lock(&pmsg->mutex);

    /* Destroy the wait conditions */
    if (pthread_cond_destroy(&pmsg->condWaitEmpty) != 0)
    {
        LOGD("OS_MsgQDelete: Could not destroy condWait\n");
    }
    if (pthread_cond_destroy(&pmsg->condWaitFull) != 0)
    {
        LOGD("OS_MsgQDelete: Could not destroy condWait\n");
    }

    /* you cannot destroy a mutex that is locked */
    pthread_mutex_unlock(&pmsg->mutex);

    if (pthread_mutex_destroy(&pmsg->mutex) != 0)
    {
        LOGD("OS_MsgQDelete: Could not destroy mutex\n");
    }

    /* DeaAllocate Buffers */
    OS_MemFree(pmsg->pbBuffers);

    /* Deallocate Record */
    OS_MemFree(pmsg);

    /* Return success */
    return (OS_OK);
}

/**
 * OS Message Queue Send function.
 *
 * @param OS_MSG_Q_ID mqId - message queue identifier
 * @param char *strMsg     - message pointer
 * @param OS_UINT uiSize   - message size
 * @param int iTimeout     - send timeout value, in ticks. (-1 = wait forever, 0 = no wait, > 1 = wait)
 * @param int iPrio        - message priority (not used)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 *    OS_TIMEOUT if a timeout occurred
 */
OS_STATUS OS_MsgQSend(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize, int iTimeout, int iPrio)
{
    OS_STATUS iRet  = OS_OK;
    OS_MBOX   *pmsg = (OS_MBOX *)mqId;
    struct timeval  tv;
    struct timespec abstime;

    /* Check the validity of message queue */
    if (mqId == 0)
    {
        LOGD("%s: BAD POINTER!\n", __FUNCTION__);
        goto error_handler;
    }

    /* Check the validity of message */
    if (strMsg == 0)
    {
        LOGD("%s: BAD POINTER!\n", __FUNCTION__);
        goto error_handler;
    }

    if (ulSize != pmsg->ulSize)
    {
        LOGD("%s:%u - Error here\n", __FILE__, __LINE__);
        goto error_handler;
    }

    /* Get Mutex */
    if (pthread_mutex_lock(&pmsg->mutex) != 0)
    {
        LOGD("%s: Failed to obtain lock\n",__FUNCTION__);
        goto error_handler;
    }

    if ( (OS_WAIT_FOREVER != iTimeout) && (OS_NO_WAIT != iTimeout) )
    {
        /* get the current time */
        if (gettimeofday(&tv, NULL) != 0)
        {
            LOGD("%s: gettimeofday Failed!\n",__FUNCTION__);
            pthread_mutex_unlock(&pmsg->mutex);
            iRet = OS_FAILURE;
            goto error_handler;
        }
        /* set our timeout */
        abstime.tv_sec  = tv.tv_sec + iTimeout/CLOCKS_PER_SEC;
        abstime.tv_nsec = ( tv.tv_usec + (iTimeout%CLOCKS_PER_SEC) ) * 1000;
    }

    /* If no space, wait */
    while ( pmsg->ulHead == ( (pmsg->ulTail + 1) % (pmsg->ulDepth + 1) ) )
    {
        if (OS_WAIT_FOREVER == iTimeout)
        {
            if (pthread_cond_wait(&pmsg->condWaitFull, &pmsg->mutex) != 0)
            {
                LOGD("OS_MsgQSend: pthread_cond_wait ERROR\n");
                iRet = OS_FAILURE;
                break;
            }
        }
        else if (OS_NO_WAIT == iTimeout)
        {
            iRet = OS_OK;
            break;
        }
        else
        {
            iRet = pthread_cond_timedwait(&pmsg->condWaitFull, &pmsg->mutex, &abstime);
            LOGD("SendMessage, iRet = %d,ETIMEDOUT = %d",iRet,ETIMEDOUT);
            if (iRet != 0)
            {
                if (iRet == ETIMEDOUT)
                {
           //         continue;
                    iRet = OS_OK;//OS_TIMEOUT;
                    break;
                }
                else
                {
                    LOGD("OS_MsgQSend: pthread_cond_timedwait ERROR = %s!\n", strerror(errno));
                    iRet = OS_FAILURE;
          //          break;
                }
            }
        }
    }

    /* If we did not time out put it in the queue */
    if (OS_OK == iRet)
//    if( pmsg->ulHead != ( (pmsg->ulTail + 1) % (pmsg->ulDepth + 1) ) )
    {
        /* Put it in the queue */
        memcpy(pmsg->pbBuffers + (pmsg->ulTail * pmsg->ulSize ), strMsg, pmsg->ulSize);
        pmsg->ulTail = ( (pmsg->ulTail + 1) % (pmsg->ulDepth + 1) );
        iRet = OS_OK;
        LOGD("OS_MsgQSend(),%s: Broadcast \n",__FUNCTION__);
        /* Signal any waiters */
        if (pthread_cond_signal(&pmsg->condWaitEmpty))
        {
            LOGD("%s: Broadcast Failed\n",__FUNCTION__);
        }
    }
    else
    {
        LOGD("%s: Broadcast Failed,MSGQ is full**********************\n",__FUNCTION__);
        iRet = -1;
    }

    /* Release mutex */
    if (pthread_mutex_unlock(&pmsg->mutex))
    {
        LOGD("%s: Failed to release lock\n",__FUNCTION__);
        goto error_handler;
    }

    /* Return success */
    return (iRet);

error_handler:

    /* Return failure */
    return (OS_FAILURE);
}

/**
 * OS Message Queue Receive function.
 *
 * @param mqId     - message queue identifier
 * @param *strMsg  - message pointer, the location of where the received message is to be placed after it has been received
 * @param uiSize   - message size (not used)
 * @param iTimeout - send timeout value, in ticks. (-1 = wait forever, 0 = no wait, > 1 = wait)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 *    OS_TIMEOUT if a timeout occurred
 */
 /*
#if defined(NDEBUG)
OS_STATUS OS_MsgQReceive(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize, int iTimeout)
#else
OS_STATUS OS_MsgQReceiveDbg(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize, int iTimeout, char *srcFile, int srcLine)
#endif*/
OS_STATUS OS_MsgQReceive(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize, int iTimeout)
{
    // Check the validity of message queue
    if (mqId == 0)
    {
#if defined(NDEBUG)
       LOGD("%s: BAD POINTER.\n", __FUNCTION__);
#else
        LOGD("%s: BAD POINTER.  Called by %s:%u\n", __FUNCTION__, srcFile, srcLine);
#endif
        return (OS_FAILURE);
    }

    // Receive message from the head by default 
    return (OS_MsgQReceiveOption(mqId, strMsg, ulSize, iTimeout, OS_MSG_Q_HEAD) );
}


/**
 * OS Message Queue Remove function.
 *
 * @param mqId     - message queue identifier
 * @param *strMsg  - message pointer, the location of where the removed message is to be placed after it has been removed
 * @param uiSize   - message size (not used)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 *    OS_TIMEOUT if msg queue is empty
 */
#if defined(NDEBUG)
OS_STATUS OS_MsgQRemove(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize)
#else
OS_STATUS OS_MsgQRemoveDbg(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize, char *srcFile, int srcLine)
#endif
{
    /* Check the validity of message queue */
    if (mqId == 0)
    {
#if defined(NDEBUG)
        LOGD("%s: BAD POINTER.\n", __FUNCTION__);
#else
        LOGD("%s: BAD POINTER.  Called by %s:%u\n", __FUNCTION__, srcFile, srcLine);
#endif
        return (OS_FAILURE);
    }

    /* Receive message from the head by default */
    return (OS_MsgQReceiveOption(mqId, strMsg, ulSize, OS_NO_WAIT, OS_MSG_Q_TAIL) );
}

/**
 * OS Message Queue Receive Option function. Same as OS_MsgQReceive, but allow message to received
 *      from either the head or the tail of the message queue.
 *
 * @param mqId     - message queue identifier
 * @param *strMsg  - message pointer, the location of where the received message is to be placed after it has been received
 * @param uiSize   - message size (not used)
 * @param iTimeout - send timeout value, in ticks. (-1 = wait forever, 0 = no wait, > 1 = wait)
 * @param iOption  - head or tail of message queue (OS_MSG_Q_HEAD or OS_MSG_Q_TAIL)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 *    OS_TIMEOUT if a timeout occurred
 */
static OS_STATUS OS_MsgQReceiveOption(OS_MSG_Q_ID mqId, char *strMsg, ULONG ulSize, int iTimeout, int iOption)
{
    OS_STATUS iRet  = OS_OK;
    OS_MBOX   *pmsg = (OS_MBOX *)mqId;
    struct timeval  tv;
    struct timespec abstime;

    /* Check the validity of message queue */
    if (mqId == 0)
    {
        LOGD("%s: INVALID mqId!\n", __FUNCTION__);
        goto error_handler;
    }

    /* Check the validity of message */
    if (strMsg == 0)
    {
        LOGD("%s: BAD POINTER!\n", __FUNCTION__);
        goto error_handler;
    }

    if (ulSize != pmsg->ulSize)
    {
        LOGD("%s: INVALID SIZE!\n", __FUNCTION__);
        goto error_handler;
    }

    /* Get Mutex */
    if (pthread_mutex_lock(&pmsg->mutex) != 0)
    {
        LOGD("%s: Failed to obtain lock\n",__FUNCTION__);
        goto error_handler;
    }

    if ( (OS_WAIT_FOREVER != iTimeout) && (OS_NO_WAIT != iTimeout) )
    {
        /* get the current time */
        if (gettimeofday(&tv, NULL) != 0)
        {
//            LOGD(("%s: gettimeofday Failed!\n",__FUNCTION__));
            pthread_mutex_unlock(&pmsg->mutex);
            iRet = OS_FAILURE;
            goto error_handler;
        }

        /* set our timeout */
        abstime.tv_sec  = tv.tv_sec + iTimeout/CLOCKS_PER_SEC;
        abstime.tv_nsec = ( tv.tv_usec + (iTimeout%CLOCKS_PER_SEC) ) * 1000;

        while ( abstime.tv_nsec > 1000000000 )
        {
            abstime.tv_sec  += 1;
            abstime.tv_nsec -= 1000000000;
        }
    }

    /* If empty queue, wait */
    while (pmsg->ulHead == pmsg->ulTail)
    {
        if (OS_WAIT_FOREVER == iTimeout)
        {
            if (pthread_cond_wait(&pmsg->condWaitEmpty, &pmsg->mutex) != 0)
            {
                LOGD("OS_MsgQReceive: pthread_cond_wait ERROR!\n");
                iRet = OS_FAILURE;
                break;
            }
        }
        else if (OS_NO_WAIT == iTimeout)
        {
            iRet = OS_TIMEOUT;
            break;
        }
        else
        {
            iRet = pthread_cond_timedwait(&pmsg->condWaitEmpty, &pmsg->mutex, &abstime);
            if (iRet != 0)
            {
                if (iRet == ETIMEDOUT)
                {
                    iRet = OS_TIMEOUT;
                    break;
                }
                else
                {
                    switch ( iRet )
                    {
                        case EINVAL:	LOGD("OS_MsgQReceive: pthread_cond_timedwait ERROR = EINVAL\n");	break;
                        case EFAULT:    LOGD ("OS_MsgQReceive: pthread_cond_timedwait ERROR = EFAULT\n");	break;
                        case ETIMEDOUT: LOGD ("OS_MsgQReceive: pthread_cond_timedwait ERROR = ETIMEDOUT\n");	break;
                       	default:		LOGD("OS_MsgQReceive: pthread_cond_timedwait ERROR = unknown\n");	break;
                    }

                    iRet = OS_FAILURE;
                    break;
                }
            }
        }
    }

    /* if we didn't time out, receive it from the queue */
    if (OS_OK == iRet)
    {
        /* Receive it from the queue */
        if (iOption == OS_MSG_Q_TAIL)
        {
            /* Receive from tail of queue */
            pmsg->ulTail = ( (pmsg->ulTail + pmsg->ulDepth) % (pmsg->ulDepth + 1) );
            memcpy(strMsg, pmsg->pbBuffers + (pmsg->ulTail * pmsg->ulSize), pmsg->ulSize);
        }
        else
        {
            /* Receive from head of queue */
            memcpy(strMsg, pmsg->pbBuffers + (pmsg->ulHead * pmsg->ulSize), pmsg->ulSize);
            pmsg->ulHead = ( (pmsg->ulHead + 1) % (pmsg->ulDepth + 1) );
        }

        /* Signal any waiters */
        if (pthread_cond_signal(&pmsg->condWaitFull))
        {
            LOGD("%s: Broadcast Failed\n",__FUNCTION__);
        }
    }

    /* Release mutex */
    if (pthread_mutex_unlock(&pmsg->mutex))
    {
        LOGD("%s: Failed to release lock\n",__FUNCTION__);
        goto error_handler;
    }

    /* Return success */
    return (iRet);

error_handler:

    /* Return failure */
    return (OS_FAILURE);
}

/**
 * Removes all messages from the queue.
 *
 * @param mqId - message queue identifier
 *
 * @retval OS_OK if successful, else an osapi error code.
 */
OS_STATUS OS_MsgQReset(OS_MSG_Q_ID mqId)
{
    OS_MBOX *pmsg = (OS_MBOX *)mqId;

    /* Check the validity of message queue */
    if (mqId == 0)
    {
        LOGD("%s: BAD POINTER!\n", __FUNCTION__);
        goto error_handler;
    }

    /* Get Mutex */
    if (pthread_mutex_lock(&pmsg->mutex) != 0)
    {
        LOGD("%s: Failed to obtain lock\n",__FUNCTION__);
        goto error_handler;
    }

    /* reset queue */
    pmsg->ulHead = pmsg->ulTail;

    /* Signal any waiters */
    if (pthread_cond_broadcast(&pmsg->condWaitEmpty))
    {
        LOGD("%s: Broadcast Failed\n",__FUNCTION__);
    }
    if (pthread_cond_broadcast(&pmsg->condWaitFull))
    {
        LOGD("%s: Broadcast Failed\n",__FUNCTION__);
    }

    /* Release mutex */
    if (pthread_mutex_unlock(&pmsg->mutex))
    {
        LOGD ("%s: Failed to release lock\n",__FUNCTION__);
        goto error_handler;
    }

    /* Return success */
    return (OS_OK);

error_handler:

    /* Return failure */
    return (OS_FAILURE);
}

/**
 * OS Message Queue Number of Messages function.
 *
 * @param
 *    OS_MSG_Q_ID mqId - message queue identifier
 *
 * @retval
 *    Returns the number of messages in the given message queue.
 */
int OS_MsgQNumMsgs(OS_MSG_Q_ID mqId)
{
    OS_MBOX *pmsg        = (OS_MBOX *)mqId;
    int     num_messages = 0;

    /* Check the validity of message queue */
    if (mqId == 0)
    {
        LOGD("%s: BAD POINTER!\n", __FUNCTION__);
        return (OS_FAILURE);
    }

    /* Get Mutex */
    if (pthread_mutex_lock(&pmsg->mutex) == 0)
    {
        if (pmsg->ulHead > pmsg->ulTail)
        {
            num_messages = pmsg->ulTail + pmsg->ulDepth + 1 - pmsg->ulHead;
        }
        else
        {
            num_messages = pmsg->ulTail - pmsg->ulHead;
        }

        /* Release mutex */
        pthread_mutex_unlock(&pmsg->mutex);
    }
    else
    {
        num_messages = OS_FAILURE;
    }

    return (num_messages);
}

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Semaphore function definitions                                           **
**                                                                           **
*******************************************************************************
******************************************************************************/

/**
 * OS Semaphore Binary Create function.
 *
 * @param
 *    int iOSOptions - operating system options (not used)
 *
 * @param
 *    OS_SEM_B_STATE semsInitialState - initial semaphore state
 *
 * @retval
 *    OS_SEM_ID semId - semaphore identifier, if successful
 *    0 - if not successful
 */
OS_SEM_ID OS_SemBCreate(int iOSOptions, OS_SEM_B_STATE semsInitialState)
{
    OS_SEMAPHORE *psemRecord = NULL;

    /* Check the validity of the initial semaphore state */
    if ( (semsInitialState != OS_SEM_EMPTY) && (semsInitialState != OS_SEM_FULL) )
    {
        LOGD("OS_SemBCreate: Invalid initial state (%d)!\n", semsInitialState);
        goto error;
    }

    /* Allocate a record for the semaphore */
    psemRecord = (OS_SEMAPHORE *)OS_MemAlloc(sizeof(OS_SEMAPHORE));
    if (psemRecord == NULL)
    {
        LOGD("OS_SemBCreate: Could not allocate semaphore record!\n");
        goto error;
    }

    /* Set the semaphore type as binary */
    psemRecord->ulSemType = OS_SEM_BINARY;

    /* Set maximum to 1  */
    psemRecord->ulMaxCount = 0;

    /* Set count to the parameter */
    psemRecord->ulCount = semsInitialState;

    /* No name was specified for the semaphore */
    strncpy(psemRecord->strName, "\0", MAX_NAME_SIZE);

    /* Create the binary semaphore */
    if (pthread_cond_init(&psemRecord->condWait, NULL) != 0)
    {
        LOGD("OS_SemBCreate: Could not init condWait");
        goto error;
    }
    if (pthread_mutex_init(&psemRecord->mutex, NULL) != 0)
    {
        LOGD("OS_SemBCreate: Could not init mutex");
        goto error;
    }

    /* Return a handle to the binary semaphore */
    return (OS_SEM_ID)psemRecord;

error:
    if (NULL != psemRecord)
    {
        OS_MemFree(psemRecord);
    }
    return (0);
}

/**
 * OS Semaphore Binary Create function.
 *
 * @param iOSOptions - operating system options (not used)
 * @param semsInitialState - initial semaphore state
 * @param strName - Null terminated string.
 *
 * @retval
 *    OS_SEM_ID semId - semaphore identifier, if successful
 *    0 - if not successful
 */
OS_SEM_ID OS_SemBCreateNamed(int iOSOptions, OS_SEM_B_STATE semsInitialState, char *strName)
{
    OS_SEM_ID semId = 0;
    OS_SEMAPHORE *psemRecord;

    /*
     *  Create a counting semaphore
     */
    semId = OS_SemBCreate(iOSOptions, semsInitialState);
    if (semId == 0)
    {
        LOGD("OS_SemBCreateNamed: Could not create named semaphore!\n");
        return 0;
    }

    /*
     *  Name the semaphore
     */
    psemRecord = (OS_SEMAPHORE *)semId;
    strncpy(psemRecord->strName, strName, MAX_NAME_SIZE);
    psemRecord->strName[MAX_NAME_SIZE-1] = 0;

    /*
     *  Return a handle to the counting semaphore
     */
    return (OS_SEM_ID)psemRecord;
}

/**
 * OS Semaphore Count Create function.
 *
 * @param
 *    int iOSOptions - operating system options
 *
 * @param
 *    int iInitialCount - initial reference count
 *
 * @retval
 *    OS_SEM_ID semId - semaphore identifier, if successful
 *    0 - if not successful
 */
OS_SEM_ID OS_SemCCreate(int iOSOptions, int iInitialCount)
{
    OS_SEMAPHORE *psemRecord = NULL;

    /* Check the validity of the given initial count */
    if (iInitialCount < 0)
    {
        LOGD("OS_SemCCreate: Invalid initial count (%d)!\n", iInitialCount);
        goto error;
    }

    /* Allocate a record for the semaphore */
    psemRecord = (OS_SEMAPHORE *)OS_MemAlloc(sizeof(OS_SEMAPHORE));
    if (psemRecord == NULL)
    {
        LOGD("OS_SemCCreate: Could not allocate semaphore record!\n");
        goto error;
    }

    /* Set the semaphore type as counting */
    psemRecord->ulSemType = OS_SEM_COUNTING;

    /* No name was specified for the semaphore */
    strncpy(psemRecord->strName, "\0", MAX_NAME_SIZE);

    /* Set maximum count to some large number... */
    psemRecord->ulMaxCount = 32678;

    /* Set count to the parameter */
    psemRecord->ulCount = iInitialCount;

    /* Create the binary semaphore */
    if (pthread_cond_init(&psemRecord->condWait, NULL) != 0)
    {
        LOGD("OS_SemCCreate: Could not init condWait");
        goto error;
    }
    if (pthread_mutex_init(&psemRecord->mutex, NULL) != 0)
    {
        LOGD("OS_SemCCreate: Could not init mutex");
        goto error;
    }

    /* Return a handle to the counting semaphore */
    return (OS_SEM_ID)psemRecord;

error:
    if (NULL != psemRecord)
    {
        OS_MemFree(psemRecord);
    }
    return (0);
}

/**
 * OS Semaphore Count Create (Named) function.
 *
 * @param
 *    int iOSOptions - operating system options
 *
 * @param
 *    int iInitialCount - initial reference count
 *
 * @param
 *    char *strName - semaphore name
 *
 * @retval
 *    OS_SEM_ID semId - semaphore identifier, if successful
 *    0 - if not successful
 */
OS_SEM_ID OS_SemCCreateNamed(int iOSOptions, int iInitialCount, char *strName)
{
    OS_SEM_ID semId = 0;
    OS_SEMAPHORE *psemRecord;

    /*
     *  Create a counting semaphore
     */
    semId = OS_SemCCreate( iOSOptions, iInitialCount );
    if (semId == 0)
    {
        LOGD("OS_SemNamedCCreate: Could not create named semaphore!\n");
        return 0;
    }

    /*
     *  Name the counting semaphore
     */
    psemRecord = (OS_SEMAPHORE *)semId;
    strncpy(psemRecord->strName, strName, MAX_NAME_SIZE);
    psemRecord->strName[MAX_NAME_SIZE-1] = 0;

    /*
     *  Return a handle to the counting semaphore
     */
    return (OS_SEM_ID)psemRecord;
}

/**
 * OS Semaphore Delete function.
 *
 * @param
 *    OS_SEM_ID semId - semaphore identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This command is always successful.
 */
#if defined(NDEBUG)
OS_STATUS OS_SemDelete(OS_SEM_ID semId)
#else
OS_STATUS OS_SemDeleteDbg(OS_SEM_ID semId, char *srcFile, int srcLine)
#endif
{
    /* Cast the given semaphore identifier to a semaphore record */
    OS_SEMAPHORE *psemRecord = (OS_SEMAPHORE *)semId;

    if (semId == 0)
    {
#if defined(NDEBUG)
        LOGD("%s: BAD POINTER.\n", __FUNCTION__);
#else
        LOGD("%s: BAD POINTER.  Called by %s:%u\n", __FUNCTION__, srcFile, srcLine);
#endif
        return (OS_FAILURE);
    }

    /* release all wait states */
    OS_SemFlush(semId);

    /* Destroy the semaphore */
    if (pthread_cond_destroy(&psemRecord->condWait) != 0)
    {
        LOGD("OS_SemDelete: Could not init condWait");
    }
    if (pthread_mutex_destroy(&psemRecord->mutex) != 0)
    {
        LOGD("OS_SemDelete: Could not init mutex");
    }

    /* Delete the semaphore record */
    OS_MemFree(psemRecord);

    /* Return success */
    return (OS_OK);
}

/**
 * OS Semaphore Take function. Basically decrements the semaphore count, if
 * possible.
 *
 * @param
 *    OS_SEM_ID semId - semaphore identifier
 *
 * @param
 *    int iTimeout - semaphore take timeout value
 *
 * @retval
 *    OS_OK if successful
 *    OS_TIMEOUT if timout
 *    OS_FAILURE if not successful
 */
#if defined(NDEBUG)
OS_STATUS OS_SemTake(OS_SEM_ID semId, int iTimeout)
#else
OS_STATUS OS_SemTakeDbg(OS_SEM_ID semId, int iTimeout, char *srcFile, int srcLine)
#endif
{
    int iRet                 = OS_OK;
    OS_SEMAPHORE *psemRecord = (OS_SEMAPHORE *)semId;
    struct timeval  tv;
    struct timespec abstime;

    /* Check the validity of the semaphore */
    if (semId == 0)
    {
#if defined(NDEBUG)
        LOGD("%s: BAD POINTER.\n", __FUNCTION__);
#else
        LOGD("%s: BAD POINTER.  Called by %s:%u\n", __FUNCTION__, srcFile, srcLine);
#endif
        goto error_handler;
    }

    /* Check the validity of the given timeout value */
    if (iTimeout < OS_WAIT_FOREVER)
    {
        LOGD("%s: Invalid timeout value (%d)!\n", __FUNCTION__, iTimeout);
        goto error_handler;
    }

    if (pthread_mutex_lock(&psemRecord->mutex) != 0)
    {
        LOGD("%s: Failed to obtain lock\n",__FUNCTION__);
        goto error_handler;
    }

    if ( (OS_WAIT_FOREVER != iTimeout) && (OS_NO_WAIT != iTimeout) )
    {
        /* get the current time */
        if (gettimeofday(&tv, NULL) != 0)
        {
            LOGD("%s: gettimeofday Failed!\n",__FUNCTION__);
            pthread_mutex_unlock(&psemRecord->mutex);
            iRet = OS_FAILURE;
            goto error_handler;
        }

        /* set our timeout */
        abstime.tv_sec  = tv.tv_sec + iTimeout/CLOCKS_PER_SEC;
        abstime.tv_nsec = ( tv.tv_usec + (iTimeout%CLOCKS_PER_SEC) ) * 1000;

        while ( abstime.tv_nsec > 1000000000 )
        {
            abstime.tv_sec  += 1;
            abstime.tv_nsec -= 1000000000;
        }
    }

    while (psemRecord->ulCount == 0)
    {
        if (OS_WAIT_FOREVER == iTimeout)
        {
            iRet = pthread_cond_wait(&psemRecord->condWait, &psemRecord->mutex);
            if (iRet != 0)
            {
                LOGD("OS_SemTake: pthread_cond_wait ERROR = %s!\n", strerror(errno));
                iRet = OS_FAILURE;
                break;
            }
        }
        else if (OS_NO_WAIT == iTimeout)
        {
            iRet = OS_TIMEOUT;
            break;
        }
        else
        {
            iRet = pthread_cond_timedwait(&psemRecord->condWait, &psemRecord->mutex, &abstime);
            if (iRet != 0)
            {
                if (iRet == ETIMEDOUT)
                {
                    iRet = OS_TIMEOUT;
                    break;
                }
                else
                {
                    LOGD("OS_SemTake: pthread_cond_timedwait ERROR = %s!\n", strerror(errno));
                    iRet = OS_FAILURE;
                    break;
                }
            }
        }
    }

    if (psemRecord->ulCount != 0)
    {
        psemRecord->ulCount--;
        iRet = OS_OK;
    }

    if (pthread_mutex_unlock(&psemRecord->mutex))
    {
        LOGD("%s: Failed to release lock\n",__FUNCTION__);
        goto error_handler;
    }

    /* Return status */
    return iRet;

error_handler:

    /* Return failure */
    return (OS_FAILURE);
}

/**
 * OS Semaphore Take function.
 *
 * @param
 *    semId - semaphore identifier
 *
 * @param
 *    ulMilliSeconds - semaphore take timeout value in milliseconds
 *
 * @retval
 *    OS_OK if successful
 *    OS_TIMEOUT if timout
 *    OS_FAILURE if not successful
 */
#if defined(NDEBUG)
OS_STATUS OS_SemTakeMsec(OS_SEM_ID semId, int milliseconds)
#else
OS_STATUS OS_SemTakeMsecDbg(OS_SEM_ID semId, int milliseconds, char* srcFile, int srcLine)
#endif
{
    if ( (milliseconds == OS_WAIT_FOREVER) || (milliseconds == OS_NO_WAIT) )
    {
        return ( OS_SemTake(semId, milliseconds) );
    }
    else
    {
        return ( OS_SemTake(semId, milliseconds*(CLOCKS_PER_SEC/1000)) );
    }
}

/**
 * OS Semaphore Give function. Basically increments the semaphore count,
 * if possible.
 *
 * @param
 *    OS_SEM_ID semId - semaphore identifier
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
#if defined(NDEBUG)
OS_STATUS OS_SemGive(OS_SEM_ID semId)
#else
OS_STATUS OS_SemGiveDbg(OS_SEM_ID semId, char *srcFile, int srcLine)
#endif
{
    OS_SEMAPHORE *psemRecord = (OS_SEMAPHORE *)semId;

    /* Check the validity of the semaphore */
    if (semId == 0)
    {
#if defined(NDEBUG)
        LOGD("%s: BAD POINTER.\n", __FUNCTION__);
#else
        LOGD("%s: BAD POINTER.  Called by %s:%u\n", __FUNCTION__, srcFile, srcLine);
#endif
        goto error_handler;
    }

    if (pthread_mutex_lock(&psemRecord->mutex) != 0)
    {
        LOGD("%s: Failed to obtain lock\n",__FUNCTION__);
        goto error_handler;
    }

    /* If the semaphore is binary and the count is 1, return an error */
    if (psemRecord->ulSemType == OS_SEM_BINARY)
    {
        if (psemRecord->ulCount == 1)
        {
            pthread_mutex_unlock(&psemRecord->mutex);
            goto error_handler;
        }
    }

    /* Increment the semaphore count */
    psemRecord->ulCount++;

    /* wake one waiting thread */
    if (pthread_cond_signal(&psemRecord->condWait))
    {
        LOGD("%s: Broadcast Failed\n",__FUNCTION__);
    }

    if (pthread_mutex_unlock(&psemRecord->mutex))
    {
        LOGD("%s: Failed to release lock\n",__FUNCTION__);
        goto error_handler;
    }

    /* Return successfully */
    return OS_OK;

error_handler:

    /* Return failure */
    return (OS_FAILURE);
}

/**
 * OS Semaphore Flush function
 *
 * @param
 *    OS_SEM_ID semId - semaphore identifier
 *
 * @retval
 *    Returns OS_OK is successful, OS_FAILURE otherwise.
 */
OS_STATUS OS_SemFlush(OS_SEM_ID semId)
{
    OS_SEMAPHORE *psemRecord = (OS_SEMAPHORE *)semId;
    OS_STATUS    status      = OS_OK;

    /* Check the validity of the semaphore */
    if (0 == semId)
    {
        LOGD(("OS_SemFlush: BAD POINTER\n"));
        return (OS_FAILURE);
    }

    if (pthread_mutex_lock(&psemRecord->mutex) == 0)
    {
        if (pthread_cond_broadcast(&psemRecord->condWait) != 0)
        {
            LOGD(("OS_SemFlush: Broadcast Failed\n"));
            status = OS_FAILURE;
        }

        pthread_mutex_unlock(&psemRecord->mutex);
    }
    else
    {
        status = OS_FAILURE;
    }

    return (status);
}


/**
 * OS memory pool Information function. Prints out information about
 * the specified memory pool.  This information includes the number of
 * pages in the memory pool, the page size, the address of each memory
 * page, and if each page is allocated or not.
 *
 * @param pool   - The memory pool ID
 *
 * @retval
 *    None.
 */
void OS_MemoryInfo(OS_MEMPOOL_ID pool)
{
    if (pool == NULL)
    {
#ifdef __CALC_PEAK_ALLOCATION
        LOGD("OS_MemoryInfo:\n"));
        LOGD("/tPeak Allocations    = %d Bytes\n", ulPeakAllocations);
        LOGD("/tCurrent Allocations = %d Bytes\n", ulCurrentAllocations);
#endif
    }
    else
    {
        OS_MEMPOOL *tmpPool = (OS_MEMPOOL *)pool;  /* Pointer to memory pool */
        CHAR       cUsed;                          /* Indicates if page allocated */
        LONG       lAtt;                           /* Indicates # pages attached */

        LOGD("\n----- Memory Pool Information -----\n");
        LOGD("Number of Pages: %d\n", tmpPool->ulNumPages);
        LOGD("Page Size:       %d\n", tmpPool->ulPgSize);
        LOGD("Memory Pages:\n");
        LOGD("\tPage\tAddress\t\tAllocated?\n");
        for (ULONG i = 0; i < tmpPool->ulNumPages; i++)
        {
            if (tmpPool->memPage[i].lAttached < 0)
            {
                lAtt  =  -1 * tmpPool->memPage[i].lAttached;
                cUsed = 'Y';
            }
            else
            {
                lAtt  = tmpPool->memPage[i].lAttached;
                cUsed = 'N';
            }
            LOGD("\t%d:\t%x\t\t%c\n", i + 1, (ULONG)tmpPool->memPage[i].pvPgAddr, cUsed);
        }
    }

    return;
}

/**
 * Creates a memory pool with the specified page size. This function does not
 * allocate the memory for the pool. The memory address is provided as an input.
 *
 * @param pAddr      - pointer to allocated memory buffer
 * @param ulSize     - Size of the memory pool
 * @param ulPageSize - Page size of allocations from the pool. All allocations
 *                     will be a multiple of this size.
 *
 * @retval non-negative memPoolID if success, NULL otherwise.
 *
 * @remark
 *    If ulSize is not a multiple of ulPageSize, then the memory pool
 *    will only contain the number of whole pages that fit in the size
 *    ulSize.
 */
OS_MEMPOOL_ID OS_CreateMemPool(PVOID pvAddr, ULONG ulSize, ULONG ulPageSize)
{
    PVOID          pvTmpAddr;           /* Address of a page in memory pool */
    OS_MEMPOOL     *tmpPool = NULL;     /* Pointer to memory pool */
    ULONG          ulArraySize;         /* Size of array to hold memory pool */

    /*
     * If any of the parameters are invalid, return.
     */
    if ( (pvAddr == NULL) || (ulPageSize == 0) )
    {
        return (NULL);
    }

    /*
     * Allocate memory for the memory pool.
     */
    tmpPool = (OS_MEMPOOL *)OS_MemAlloc(sizeof(OS_MEMPOOL) );
    if (tmpPool == NULL)
    {
        return (NULL);
    }

    /*
     * Set value for memory pool page size and the number of pages.
     */
    tmpPool->ulPgSize   = ulPageSize;
    tmpPool->ulNumPages = ulSize / ulPageSize;

    /*
     * Allocate an array large enough to hold the memory pool.
     */
    ulArraySize         = sizeof(OS_MEMPOOL::Page) * tmpPool->ulNumPages;
    tmpPool->memPage    = (OS_MEMPOOL::Page *)OS_MemAlloc(ulArraySize);
    if (tmpPool->memPage == NULL)
    {
        (void)OS_MemFree(tmpPool);
        return (NULL);
    }

    /*
     * Give a memory address to each page in the memory pool.  Also,
     * each page is given a value of the number of pages attached to
     * it, including itself.
     */
    pvTmpAddr = pvAddr;
    for (ULONG i = 0; i < tmpPool->ulNumPages; i++)
    {
        tmpPool->memPage[i].pvPgAddr   = pvTmpAddr;
        tmpPool->memPage[i].lAttached  = tmpPool->ulNumPages - i;
        pvTmpAddr                      = (BYTE *)pvTmpAddr + ulPageSize;
    }

    return ( (OS_MEMPOOL_ID)tmpPool);
}

/**
 * Deletes a memory pool created by OS_CreateMemPool. This does not release the memory
 * buffer used by the pool. This memory should be released by the application.
 *
 * @param pool   - The memory pool ID
 *
 * @retval OS_OK if successful, an osapi error code otherwise.
 */
OS_STATUS OS_DeleteMemPool(OS_MEMPOOL_ID pool)
{
    OS_MEMPOOL *tmpPool = (OS_MEMPOOL *)pool;   /* Pointer to memory pool */

    /*
     * If memory pool ID is NULL, then return.
     */
    if (pool == NULL)
    {
        return (OS_INVALID_MEMPOOL);
    }

    /*
     * Free any memory used by memory pool.
     */
    OS_MemFree(tmpPool->memPage);
    OS_MemFree(pool);

    return (OS_OK);
}

/**
 * Allocates a block of memory from the specified memory pool.
 *
 * @param pool   - The memory pool ID
 * @param ulSize - Size of memory block to allocate.
 *
 * @retval - pointer to allocated memory if successful, NULL if not successful
 */
PVOID OS_MemPoolAlloc(OS_MEMPOOL_ID pool, ULONG ulSize)
{
    LONG        lPgNeeded;                     /* Number of pages needed to allocate the *
                                                * requested block of memory.             */
    OS_MEMPOOL  *tmpPool = (OS_MEMPOOL *)pool; /* Pointer to memory pool */
    ULONG i;                                   /* Loop iterator */

    /*
     * If invalid memory pool, return.
     */
    if (pool == NULL)
    {
        return NULL;
    }

    /*
     * Determine how many pages are needed for the requested amount
     * of memory.
     */
    lPgNeeded = (ulSize / tmpPool->ulPgSize);
    if (ulSize % tmpPool->ulPgSize != 0)
    {
        ++lPgNeeded;
    }

    /* Search for a block of pages that have at least the number
     * of pages needed consecutively.  Once a block is found, return
     * the address of the first page of this block. If no blocks large
     * enough are found, then return NULL.
     */
    i = 0;
    while (i < tmpPool->ulNumPages)
    {
        /*
         * Look at the number of pages attached to the current page to
         * determine if the block of pages is large enough to use.  If
         * the number of pages attached is a negative value, then move ahead
         * that number of pages (positive value) because those pages are allocated.
         * If the number of pages attached is less than the number of pages
         * needed, then move ahead that number of pages because the block of
         * pages is not large enough.  Otherwise, the block is large enough,
         * so allocate a block starting at current page.
         */
        if (tmpPool->memPage[i].lAttached < 0)
        {
            i = i + (-1 * tmpPool->memPage[i].lAttached);
        }
        else if (tmpPool->memPage[i].lAttached < lPgNeeded)
        {
            i = i + tmpPool->memPage[i].lAttached;
        }
        else
        {
            /*
             * Adjust the number of pages attached to the allocated pages.
             * Simply change number to negative.
             */
            LONG lPages = lPgNeeded;
            for (ULONG j = i; j < (i + lPgNeeded); j++)
            {
                tmpPool->memPage[j].lAttached = -1 * lPages;
                lPages = lPages - 1;
            }

            return (tmpPool->memPage[i].pvPgAddr);
        }
    }

    return (NULL);
}

/**
 * Frees the given memory block back to the specified memory pool
 *
 * @param pool   - The memory pool ID
 * @param pvMemory - The address
 *
 * @retval - OS_OK if sucessful, an osapi error code otherwise.
 */
OS_STATUS OS_MemPoolFree(OS_MEMPOOL_ID pool, PVOID pvMemory)
{
    LONG        lIndex,                          /* Page number in memory pool */
                lNextIndex;                      /* Page number of next block  */
    OS_MEMPOOL  *tmpPool = (OS_MEMPOOL *)pool;   /* Pointer to memory pool */
    ULONG       ulFront,                         /* Beginning and end address of */
                ulEnd;                           /* memory pool.                 */
    /*
     * If null pointer was passed, return.
     */
    if (pvMemory == NULL)
    {
        return (OS_NULL_POINTER);
    }

    /*
     * Check that memory is in the specified memory pool. If it's not
     * then do not free the memory.
     */
    ulFront = (ULONG)tmpPool->memPage[0].pvPgAddr;
    ulEnd   = (ULONG)tmpPool->memPage[tmpPool->ulNumPages - 1].pvPgAddr +
              tmpPool->ulPgSize;
    if ( ( (ULONG)pvMemory < ulFront) || ( (ULONG)pvMemory >= ulEnd) ||
         (pool == NULL) )
    {
        return (OS_INVALID_MEMPOOL);
    }

    /*
     * Determine, from the address, which page(s) to free.  If address
     * is not a page address, then return.
     */
    lIndex     = (BYTE *)pvMemory - (BYTE *)tmpPool->memPage[0].pvPgAddr;
    if (lIndex % tmpPool->ulPgSize != 0)
    {
        return (OS_FAILURE);
    }
    lIndex     = lIndex / tmpPool->ulPgSize;

    /*
     * Make sure that the memory address given is not already free.
     */
    if (tmpPool->memPage[lIndex].lAttached >= 0)
    {
        return (OS_FAILURE);
    }

    /*
     * Adjust the number of pages attached to the freed page(s).
     */
    lNextIndex = lIndex + (-1 * tmpPool->memPage[lIndex].lAttached);
    LONG i;
    if (tmpPool->memPage[lNextIndex].lAttached > 0)
    {
        /*
         * Need to add the number of pages attached to the pages
         * following the freed pages.
         */
        for (i = lIndex; i < lNextIndex; i++)
        {
            tmpPool->memPage[i].lAttached =
                    (-1 * tmpPool->memPage[i].lAttached) +
                    tmpPool->memPage[lNextIndex].lAttached;
        }
    }
    else
    {
        /*
         * No non-allocated pages following the freed pages.
         */
        for (i = lIndex; i < lNextIndex; i++)
        {
            tmpPool->memPage[i].lAttached =
                    (-1 * tmpPool->memPage[i].lAttached);
        }
    }

    /*
     * Adjust the number of pages attached to any pages preceding the
     * freed pages.
     */
    i = lIndex - 1;
    while ( (tmpPool->memPage[i].lAttached >= 0) && (i >= 0) )
    {
        tmpPool->memPage[i].lAttached += tmpPool->memPage[lIndex].lAttached;
        i = i - 1;
    }

    return (OS_OK);
}

/**
 * Frees all memory blocks back to the specified memory pool
 *
 * @param pool   - The memory pool ID
 *
 * @retval - OS_OK if sucessful, an osapi error code otherwise.
 */
OS_STATUS OS_MemPoolReset(OS_MEMPOOL_ID pool)
{
    OS_MEMPOOL *pMemPool = (OS_MEMPOOL *)pool;

    if (pool == NULL)
    {
        LOGD("OS_MemPoolReset: NULL pointer!\n");
        return (OS_NULL_POINTER);
    }

    /* Free all pages of mempool */
    for (ULONG i = 0; i < pMemPool->ulNumPages; i++)
    {
        pMemPool->memPage[i].lAttached  = pMemPool->ulNumPages - i;
    }

    return (OS_OK);
}

/**
 * Allocates a block of memory from the OS memory pool.
 *
 * @param ulSize - memory size.
 *
 * @retval
 *    pvMemory - pointer to allocated memory, if successful
 *    NULL - if not successful
 */
#ifndef DMALLOC
PVOID OS_MemAlloc(ULONG ulSize)
{
    PVOID pvMemory = NULL;

#ifdef __CALC_PEAK_ALLOCATION
    if (fFirstAlloc == TRUE)
    {
        memset(test, 0, sizeof(mem_info) * 512);
        fFirstAlloc = FALSE;
    }
#endif

    /*
     *  Check the validity of the given memory size
     */
    if (ulSize == 0)
    {
        LOGD(("OS_MemAlloc: Memory size is 0!\n"));
        return (NULL);
    }

    /*
     *  Allocate a block of the given size from the OS memory
     */
    pvMemory = malloc(ulSize);

#ifdef __CALC_PEAK_ALLOCATION
    if (pvMemory != NULL)
    {
        for (int i=0; i<512; i++)
        {
            if (test[i].pvAddr == NULL)
            {
                test[i].pvAddr = pvMemory;
                test[i].size   = ulSize;
                break;
            }
        }

        ulCurrentAllocations += ulSize;
        if (ulPeakAllocations < ulCurrentAllocations)
        {
            ulPeakAllocations = ulCurrentAllocations;
        }
    }
#endif

//    DbgAssert(pvMemory != NULL);

    /*
     *  Return a pointer to the allocated block
     */
    return (pvMemory);
}

/**
 * Frees the given memory block back to the OS memory pool.
 *
 * @param pvMemory - pointer to the memory to be freed
 *
 * @retval OS_OK if successful, otherwise an osapi error code
 */
OS_STATUS OS_MemFree(PVOID pvMemory)
{
    /*
     *  Check the validity of the given memory pointer
     */
    if (pvMemory == NULL)
    {
        LOGD("OS_MemFree: Memory pointer is NULL!\n");
        return (OS_NULL_POINTER);
    }

#ifdef __CALC_PEAK_ALLOCATION
    for (int i=0; i<512; i++)
    {
        if (test[i].pvAddr == pvMemory)
        {
            ulCurrentAllocations -= test[i].size;

            test[i].pvAddr = NULL;
            test[i].size   = 0;
            break;
        }
    }
#endif

    /*
     *  Make sure the OS memory pool has been created
     */

    /*
     *  Free the given block back to the OS memory
     */
    free(pvMemory);

    return (OS_OK);
}
#endif

/**
 * OS Initialize DRAM Top function.
 *
 * @param
 *    None.
 *
 * @retval
 *    OS_MEMORY_BASE - offset to the top of DRAM, if successful
 *    NULL - if not successful
 */
OS_STATUS OS_InitDramTop(PVOID pStartAddr, ULONG ulMemSize)
{
    return (OS_NOT_IMPLEMENTED);
}

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Timer function prototypes                                                **
**                                                                           **
*******************************************************************************
******************************************************************************/

#ifdef VC_TIMERS

typedef struct tagThreadTimer
{
    int iThreadID;              /* the ID of the thread in charge of this timer */
    timer_t timerid;            /* the slot value and unique timer id */
    struct sigevent evp;        /* description of event, only callback type supported */
    char strThreadName[17];     /* name of thread, this shall be VCTimerThreadXXX */
    struct itimerspec itimer;   /* this contains the timeout or repeat value */
    OS_SEM_ID semTimerSet;      /* used to block for timeout specified */
    BOOLEAN fTimerSetPending;   /* set when timer set is pending / cleared once timer starts counting */
    BOOLEAN fTimerCounting;     /* set while timer is counting */
    BOOLEAN fTimerExitPending;  /* set when timer exit is pending */
    struct timespec abstime;
} ThreadTimer;

#define TOTAL_TIMERS 200
#define VC_TIMER_THREAD_STACK_SIZE  1024

/* @todo Track the timer list with a dynamic clist */
ThreadTimer *pttMasterList[TOTAL_TIMERS] = { NULL };

/**
 * VCI Pthread-based, Posix-style Timer
 *
 * @param
 *    clockid_t clock_id
 *
 * @param
 *    struct sigevent *evp
 *
 * @param
 *    timer_t *timerid

 * @retval
 *    0 if successful
 *    -1 if not successful
 */
int vc_timer_create( clockid_t clockid, struct sigevent *evp, timer_t *timerid )
{

    int iReturn = 0;
    int i = 0;
    char *strThreadName = "VCTimerThread";
    ThreadTimer *pttThreadTimer = NULL;
    BOOLEAN fFoundOpen = FALSE;

    /* we only handle REALTIME at this time */
    if( CLOCK_REALTIME != clockid )
    {
        LOGD("%s - clockid problem %08X\n", __FUNCTION__, clockid);
        iReturn = -1;
        goto leave;
    }

    /* we only treat call-back signals */
    if( NULL == evp )
    {
        LOGD("%s - NULL POINTER\n", __FUNCTION__ );
        iReturn = -1;
        goto leave;
    }

    /* malloc a structure to store all of the timer info including the threadid, timerid, and all of the private params */
    if( NULL == ( pttThreadTimer = (ThreadTimer*)OS_MemAlloc(sizeof(ThreadTimer)) ) )
    {
        LOGD("%s - OUT OF MEMORY\n", __FUNCTION__);
        iReturn = -1;
        goto leave;
    }

    /* @todo - add new pThreadTimer to a list of some sort(possibly a clist) */
    /* find a free slot in the global timer storage */
    for (i = 0; i < TOTAL_TIMERS; i++)
    {
        if (pttMasterList[i] == NULL)
        {
            pttMasterList[i] = pttThreadTimer;
            fFoundOpen = TRUE;
            break;
        }
    }

    if( FALSE == fFoundOpen )
    {
        LOGD("%s - can't find an open timer slot\n", __FUNCTION__);
        iReturn = -1;
        goto leave;
    }

    /* Intially timer is waiting for a timer to be set and not counting */
    pttThreadTimer->fTimerExitPending = FALSE;
    pttThreadTimer->fTimerSetPending  = FALSE;
    pttThreadTimer->fTimerCounting    = FALSE;

    /* This is a semaphore that allows the thread to wait for the time that timer is set to */
    pttThreadTimer->semTimerSet = OS_SemBCreate(OS_SEM_Q_FIFO, OS_SEM_EMPTY);

    /* The evp contains the function that gets called from the timer and its context variable */
    pttThreadTimer->evp = *evp;

    /* Store the global index value for referring to this timer in the future */
    *timerid = (timer_t)i;
    pttThreadTimer->timerid = (timer_t)i;

    /* Set a unique number for each new thread timer */
    snprintf( pttThreadTimer->strThreadName, sizeof( pttThreadTimer->strThreadName ), "%s%02d", strThreadName, i );

    /* Spawn a new task with high priority to act as a timer handler */
    if( OS_FAILURE == ( pttThreadTimer->iThreadID = OS_TaskSpawnParam( pttThreadTimer->strThreadName,
                                                                       OS_TASK_HIGH_PRIORITY,
                                                                       VC_TIMER_THREAD_STACK_SIZE,
                                                                       TimerThreadProc,
                                                                       pttThreadTimer, NULL ) ) )
    {
        LOGD("%s - iThreadID problem %08X\n", __FUNCTION__, pttThreadTimer->iThreadID);
        iReturn = -1;
        goto leave;
    }

    LOGD("%s - %d Launched %s with PID %d\n", __FUNCTION__, pttThreadTimer->timerid,
             pttThreadTimer->strThreadName, pttThreadTimer->iThreadID);

leave:

    if( 0 != iReturn )
    {
        free( pttThreadTimer );
        pttThreadTimer = NULL;

        if( TRUE == fFoundOpen )
        {
            pttMasterList[(int)*timerid] = NULL;
        }
    }

    return( iReturn );

} /* end vc_timer_create() */

/**
 * VCI Pthread-based, Posix-style Timer
 *
 * @param
 *    timer_t timerid
 *
 * @retval
 *    0 if successful
 *    -1 if not successful
 */
int vc_timer_delete( timer_t timerid )
{
    int iReturn = 0;
    ThreadTimer *pttThreadTimer;

    /* get the right record */
    if( NULL == ( pttThreadTimer = pttMasterList[(int)timerid] ) )
    {
        LOGD("%s - invalid timerid %08X\n", __FUNCTION__, timerid);
        iReturn = -1;
        goto leave;
    }

    LOGD("%s - Deleting timer %d\n", __FUNCTION__, timerid);

    /* timer should stop counting and exit */
    pttThreadTimer->fTimerSetPending  = FALSE;
    pttThreadTimer->fTimerCounting    = FALSE;
    pttThreadTimer->fTimerExitPending = TRUE;

    /* zero out the time values */
    pttThreadTimer->itimer.it_value.tv_sec = 0;
    pttThreadTimer->itimer.it_value.tv_nsec = 0;
    pttThreadTimer->itimer.it_interval.tv_sec = 0;
    pttThreadTimer->itimer.it_interval.tv_nsec = 0;

    /* make sure timer is not counting or waiting for a time to be set */
    OS_SemGive(pttThreadTimer->semTimerSet);

    /* delete the timer thread */
    OS_TaskJoin(pttThreadTimer->iThreadID);
    OS_TaskDelete(pttThreadTimer->iThreadID);

    /* if this fails, continue to muscle through the rest of deletion */
    if ( OS_FAILURE == OS_SemDelete( pttThreadTimer->semTimerSet ) )
    {
        LOGD("%s - OS_SemDelete failure %d\n", __FUNCTION__, pttThreadTimer->iThreadID);
        iReturn = -1;
    }

    /* free the structure for this timerthread */
    free( pttThreadTimer );
    pttThreadTimer = NULL;

    /* remove it from the global list */
    pttMasterList[(int)timerid] = NULL;

leave:

    return( iReturn );

} /* end vc_timer_delete() */

/**
 * VCI Pthread-based, Posix-style Timer
 *
 * @param
 *    timer_t timerid
 *
 * @param
 *    int flags
 *
 * @param
 *    const struct itimerspec *value
 *
 * @param
 *    struct itimerspec *ovalue
 *
 * @retval
 *    0 if successful
 *    -1 if not successful
 */
int vc_timer_settime( timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue )
{
    int          iReturn = 0;
    ThreadTimer  *pttThreadTimer;
    OS_SEMAPHORE *psemRecord;

    if (NULL == value)
    {
        LOGD("%s - NULL POINTER\n", __FUNCTION__);
        iReturn = -1;
        goto leave;
    }

    /* get the right record */
    if( NULL == ( pttThreadTimer = pttMasterList[(int)timerid] ) )
    {
        LOGD("%s - invalid timerid %08X\n", __FUNCTION__, timerid);
        iReturn = -1;
        goto leave;
    }

    /* Check the validity of the semaphore */
    if (pttThreadTimer->semTimerSet == 0)
    {
        LOGD("%s - Failure, NULL semaphore\n", __FUNCTION__);
        iReturn = -1;
        goto leave;
    }

    /* init our local semaphore pointer */
    psemRecord = (OS_SEMAPHORE *)pttThreadTimer->semTimerSet;

    /* lock things down */
    if (pthread_mutex_lock(&psemRecord->mutex) != 0)
    {
        LOGD("%s: Failed to obtain lock\n",__FUNCTION__);
        iReturn = -1;
        goto leave;
    }

    /* this is going to set the timer expiration value for select() */
    pttThreadTimer->itimer = *value;

    if( ( pttThreadTimer->itimer.it_value.tv_sec == 0 ) &&
        ( pttThreadTimer->itimer.it_value.tv_nsec == 0 ) &&
        ( pttThreadTimer->itimer.it_interval.tv_sec == 0 ) &&
        ( pttThreadTimer->itimer.it_interval.tv_nsec == 0 ) )
    {
        /*
         * Time was set to 0, which means to stop the timer. If the timer
         * is currently couting, then we want it to stop counting.  Otherwise,
         * just let timer to continue to wait until it is set to a non-zero value.
         */

        if (pttThreadTimer->fTimerCounting)
        {
            pttThreadTimer->fTimerSetPending = FALSE;
            pttThreadTimer->fTimerCounting   = FALSE;

            if (psemRecord->ulCount == 0)
            {
                /* Increment the semaphore count */
                psemRecord->ulCount++;

                /* wake the timer thread */
                pthread_cond_signal(&psemRecord->condWait);
            }
        }
        else if (pttThreadTimer->fTimerSetPending)
        {
            pttThreadTimer->fTimerSetPending = FALSE;
        }
    }
    else
    {
        /*
         * Time was set to a non-zero value.
         * If the timer is not currently counting, then it will start counting.
         * If the timer is counting, then it will continue to count, but it will
         * count from the new time value.
         */
        pttThreadTimer->fTimerSetPending = TRUE;

        if (psemRecord->ulCount == 0)
        {
            /* Increment the semaphore count */
            psemRecord->ulCount++;

            /* wake the timer thread */
            pthread_cond_signal(&psemRecord->condWait);
        }
    }

    /* release the mutex */
    pthread_mutex_unlock(&psemRecord->mutex);

leave:

    return( iReturn );

} /* end vc_timer_settime() */

/**
 * VCI Pthread-based, Posix-style Timer
 *
 * @param
 *    timer_t timerid
 *
 * @param
 *    struct itimerspec *value
 *
 * @retval
 *    0 if successful
 *    -1 if not successful
 */
int vc_timer_gettime( timer_t timerid, struct itimerspec *ptimerspec )
{
    int            iReturn = 0;
    struct timeval time_start;
    ThreadTimer    *pttThreadTimer;
    OS_SEMAPHORE   *psemRecord;

    if (NULL == ptimerspec)
    {
        LOGD("%s - NULL POINTER\n", __FUNCTION__);
        iReturn = -1;
        goto leave;
    }

    /* get the right record */
    if ( NULL == ( pttThreadTimer = pttMasterList[(int)timerid] ) )
    {
        LOGD("%s - invalid timerid %08X\n", __FUNCTION__, timerid);
        iReturn = -1;
        goto leave;
    }

    /* Check the validity of the semaphore */
    if (pttThreadTimer->semTimerSet == 0)
    {
        LOGD("%s - Failure, NULL semaphore\n", __FUNCTION__);
        iReturn = -1;
        goto leave;
    }

    /* init our local semaphore pointer */
    psemRecord = (OS_SEMAPHORE *)pttThreadTimer->semTimerSet;

    /* lock things down */
    if (pthread_mutex_lock(&psemRecord->mutex) != 0)
    {
        LOGD("%s: Failed to obtain lock\n",__FUNCTION__);
        iReturn = -1;
        goto leave;
    }

    /* if there is a set pending then just take the time from that */
    if (pttThreadTimer->fTimerSetPending)
    {
        ptimerspec->it_value.tv_sec  = pttThreadTimer->itimer.it_value.tv_sec;
        ptimerspec->it_value.tv_nsec = pttThreadTimer->itimer.it_value.tv_nsec;

        /* release the mutex */
        pthread_mutex_unlock(&psemRecord->mutex);
        goto leave;
    }

    if ( ( pttThreadTimer->itimer.it_value.tv_sec == 0 ) &&
         ( pttThreadTimer->itimer.it_value.tv_nsec == 0 ) &&
         ( pttThreadTimer->itimer.it_interval.tv_sec == 0 ) &&
         ( pttThreadTimer->itimer.it_interval.tv_nsec == 0 ) )
    {
        /* timer is stopped */
        ptimerspec->it_interval.tv_sec  = 0;
        ptimerspec->it_interval.tv_nsec = 0;
        ptimerspec->it_value.tv_sec     = 0;
        ptimerspec->it_value.tv_nsec    = 0;
    }
    else
    {
        /* get the current time */
        if (gettimeofday(&time_start, NULL) == 0)
        {
            /* return the timer info */
            ptimerspec->it_interval.tv_sec  = pttThreadTimer->itimer.it_interval.tv_sec;
            ptimerspec->it_interval.tv_nsec = pttThreadTimer->itimer.it_interval.tv_nsec;

            /* projected time must be greater than current for comparison */
            if (pttThreadTimer->abstime.tv_sec > time_start.tv_sec)
            {
                ptimerspec->it_value.tv_sec  = pttThreadTimer->abstime.tv_sec  - time_start.tv_sec;
                ptimerspec->it_value.tv_nsec = pttThreadTimer->abstime.tv_nsec - (time_start.tv_usec * 1000);
            }
            else if (pttThreadTimer->abstime.tv_nsec > (time_start.tv_usec * 1000))
            {
                ptimerspec->it_value.tv_sec  = pttThreadTimer->abstime.tv_sec  - time_start.tv_sec;
                ptimerspec->it_value.tv_nsec = pttThreadTimer->abstime.tv_nsec - (time_start.tv_usec * 1000);
            }
            else
            {
                /* already expired */
                ptimerspec->it_value.tv_sec  = 0;
                ptimerspec->it_value.tv_nsec = 0;
            }
        }
        else
        {
            iReturn = -1;
        }
    }

    /* release the mutex */
    pthread_mutex_unlock(&psemRecord->mutex);

leave:

    return( iReturn );

} /* end vc_timer_gettime() */

/**
 * VCI Pthread-based, Posix-style Timer
 *
 * @param
 *    PVOID pvParam
 *
 * @retval
 *    0 if successful
 *    -1 if not successful
 */
ULONG TimerThreadProc( PVOID pvParam )
{
    ThreadTimer  *pttThreadTimer = (ThreadTimer*)pvParam;
    struct       timeval  time;
    struct       timeval  time_start;
    BOOLEAN      fRepeat = FALSE;
    OS_SEMAPHORE *psemRecord;

    if (NULL == pttThreadTimer)
    {
        LOGD("%s - Failure, NULL pttThreadTimer\n", __FUNCTION__);
        goto leave;
    }

    /* Check the validity of the semaphore */
    if (pttThreadTimer->semTimerSet == 0)
    {
        LOGD("%s - Failure, NULL semaphore\n", __FUNCTION__);
        goto leave;
    }

    /* init our local semaphore pointer */
    psemRecord = (OS_SEMAPHORE *)pttThreadTimer->semTimerSet;

    /* lock things down */
    if (pthread_mutex_lock(&psemRecord->mutex) != 0)
    {
        LOGD("%s: Failed to obtain lock\n",__FUNCTION__);
        goto leave;
    }

    /*
     * Do not exit task until the timer has been deleted.  Each time a timer is expired,
     * just continue to wait until a new value is set or until the timer has been deleted.
     */
    do
    {
        /* wait until the timer rate has been issued */
        while (psemRecord->ulCount == 0)
        {
            /* wait forever */
            if (pthread_cond_wait(&psemRecord->condWait, &psemRecord->mutex) != 0)
            {
                /* kick out due to an error */
                break;
            }
        }
        if (psemRecord->ulCount != 0)
        {
            psemRecord->ulCount--;
        }

        /* If a timer set is pending, then proceed to start the timer */
        if ( (pttThreadTimer->fTimerSetPending == TRUE) && (pttThreadTimer->fTimerExitPending == FALSE) )
        {
            /*
             * Continue counting while there is a repeat time value set, or while
             * new time values are being set.
             */
            do
            {
                /* get the current time */
                if (gettimeofday(&time_start, NULL) != 0)
                {
                    LOGD("%s: gettimeofday Failed!\n",__FUNCTION__);
                    OS_TaskYield();
                    continue;
                }

                /* check to see if the timer variables need set */
                if (pttThreadTimer->fTimerSetPending)
                {
                    /* set the initial absolute time */
                    pttThreadTimer->abstime.tv_sec  = time_start.tv_sec;
                    pttThreadTimer->abstime.tv_nsec = time_start.tv_usec * 1000;

                    /* check to see if it has been updated */
                    time.tv_sec  = pttThreadTimer->itimer.it_value.tv_sec;
                    time.tv_usec = pttThreadTimer->itimer.it_value.tv_nsec/1000;

                    /* is this a repeating timer? */
                    if ( (pttThreadTimer->itimer.it_interval.tv_sec > 0) ||
                        (pttThreadTimer->itimer.it_interval.tv_nsec > 0) )
                    {
                        fRepeat = TRUE;
                    }
                    else
                    {
                        fRepeat = FALSE;
                    }

                    pttThreadTimer->fTimerSetPending = FALSE;
                    pttThreadTimer->fTimerCounting   = TRUE;
                }

                /* increment the absolute time each loop by the timer delay length. Then use this as
                 * the absolute timeout of the semaphore below. This will make sure the timer rate
                 * is not skewed by the processing time of the timer callback */
                pttThreadTimer->abstime.tv_sec  += time.tv_sec;
                pttThreadTimer->abstime.tv_nsec += time.tv_usec * 1000;
                if (pttThreadTimer->abstime.tv_nsec > 1000000000)
                {
                    pttThreadTimer->abstime.tv_sec  += 1;
                    pttThreadTimer->abstime.tv_nsec -= 1000000000;
                }

                /* if the absolute time has already passed then we have to reset
                 * which will result in a missed timer fire. To minimize this the timer callback
                 * should execute very quickly. */
                if ( (time_start.tv_sec > pttThreadTimer->abstime.tv_sec) ||
                    ( ((time_start.tv_usec * 1000) > pttThreadTimer->abstime.tv_nsec) &&
                    (time_start.tv_sec == pttThreadTimer->abstime.tv_sec) ) )
                {
                    /* set the initial absolute time */
                    pttThreadTimer->abstime.tv_sec  = time_start.tv_sec + time.tv_sec;
                    if (time_start.tv_usec + time.tv_usec > 1000000)
                    {
                        pttThreadTimer->abstime.tv_sec++;
                        pttThreadTimer->abstime.tv_nsec = ( (time_start.tv_usec + time.tv_usec) * 1000 ) - 1000000;
                }
                    else
                {
                    pttThreadTimer->abstime.tv_nsec = (time_start.tv_usec + time.tv_usec) * 1000;
                }
                }

                /*
                 * Wait for the specified amount of time.  If the timer is set again while we
                 * are waiting, then we will stop waiting and start waiting for the new specified
                 * amount of time.  OS_TaskDelay() will not work here because if a new time is
                 * set, it will not know to restart the timer at the new time value.
                 */
                while (psemRecord->ulCount == 0)
                {
                    if (pthread_cond_timedwait(&psemRecord->condWait, &psemRecord->mutex, &pttThreadTimer->abstime) != 0)
                    {
                        /* kick out due to timeout or error */
                        break;
                    }
                }
                if (psemRecord->ulCount != 0)
                {
                    psemRecord->ulCount--;
                }

                /* Send a callback.
                 * If the timer is in the process of exiting, do not send a callback.
                 * If the timer has been set to a new value (timer set pending), then
                 * do not send a callback.
                 */
                if ( (NULL != pttThreadTimer->evp.sigev_notify_function) &&
                     !(pttThreadTimer->fTimerExitPending || pttThreadTimer->fTimerSetPending) &&
                     (pttThreadTimer->fTimerCounting == TRUE) )
                {
                    /* unlock mutex during the callback to prevent possible deadlocks */
                    pthread_mutex_unlock(&psemRecord->mutex);

                    if (NULL != pttThreadTimer->evp.sigev_value.sival_ptr)
                    {
                        ( (OS_FUNCPTR_PARAM)(pttThreadTimer->evp.sigev_notify_function) )(pttThreadTimer->evp.sigev_value.sival_ptr);
                    }
                    else
                    {
                        ( (OS_FUNCPTR)(pttThreadTimer->evp.sigev_notify_function) )();
                    }

                    /* reaquire lock */
                    pthread_mutex_lock(&psemRecord->mutex);
                }

            } while ( ( (fRepeat == TRUE) || (pttThreadTimer->fTimerSetPending == TRUE) ) &&
                (pttThreadTimer->fTimerExitPending == FALSE) );

            /* Timer is no longer counting */
            pttThreadTimer->fTimerCounting = FALSE;
        }

    } while (pttThreadTimer->fTimerExitPending == FALSE);

    /* release the mutex */
    pthread_mutex_unlock(&psemRecord->mutex);

leave:

    pthread_exit(0);
    return (OS_OK);

} /* end TimerThreadProc() */

#endif

/**
 * OS Timer Create function
 *
 * @param
 *    OS_TIMER_ID **ptimerId - pointer to an timer identifier pointer
 *
 * @param
 *    OS_FUNCPTR func - timer procedure function
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerCreate(OS_TIMER_ID *ptimerId, OS_FUNCPTR func)
{
    return (OS_TimerCreateParam(ptimerId, (OS_FUNCPTR_PARAM)func, NULL));
}

/**
 * OS Timer Create function
 *
 * @param
 *    OS_TIMER_ID **ptimerId - pointer to an timer identifier pointer
 *
 * @param
 *    OS_FUNCPTR func - timer procedure function
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerCreateParam(OS_TIMER_ID *ptimerId, OS_FUNCPTR_PARAM func, PVOID pvParam)
{
    OS_TIMER          *pTimer = NULL;
    OS_STATUS         status  = OS_OK;

    /* allocate the timer structure */
    pTimer = (OS_TIMER*)OS_MemAlloc(sizeof(OS_TIMER));
    if (pTimer != NULL)
    {
        struct sigevent sigev;
        memset (&sigev, 0, sizeof (struct sigevent));

        pTimer->pvFunction = (PVOID)func;
        pTimer->pvParam    = pvParam;

        /* info to pass to the callback function */
        sigev.sigev_value.sival_int = 0;
        sigev.sigev_value.sival_ptr = pTimer;

        sigev.sigev_notify = SIGEV_THREAD;
        sigev.sigev_notify_attributes = NULL;
        sigev.sigev_notify_function = linuxTimer;

        if (timer_create(CLOCK_REALTIME, &sigev, &pTimer->timerID) < 0)
        {

            LOGD ("OS_TimerCreate: FAILED\n");

            /* free memory */
            OS_MemFree(pTimer);

            /* return an error */
            status = OS_FAILURE;

        }
        else
        {
            *ptimerId = (OS_TIMER_ID)pTimer;
        }
    }
    else
    {
        status = OS_NO_MEMORY;
    }

    return (status);
}

/**
 * OS Timer Delete function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerDelete(OS_TIMER_ID ptimerId)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerId;
    OS_STATUS status  = OS_OK;

    if (pTimer != NULL)
    {
        if (timer_delete(pTimer->timerID) < 0)
        {
            status = OS_FAILURE;
            LOGD("OS_TimerDelete: FAILED\n");
        }

        /* free memory */
        OS_MemFree(pTimer);
    }
    else
    {
        status = OS_FAILURE;
        LOGD("OS_TimerDelete: FAILED\n");
    }


    return (status);
}

/**
 * OS Timer Stop function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerStop(OS_TIMER_ID ptimerId)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerId;
    OS_STATUS status  = OS_OK;

    if (NULL != pTimer)
    {
        struct itimerspec itimer;

        /* setting the timer period to 0 stop it */
        itimer.it_interval.tv_sec  = 0;
        itimer.it_interval.tv_nsec = 0;
        itimer.it_value.tv_sec     = 0;
        itimer.it_value.tv_nsec    = 0;

        /* add the timer */
        if (timer_settime (pTimer->timerID, 0, &itimer, NULL) < 0)
        {
            status = OS_FAILURE;
        }
    }
    else
    {
        status = OS_FAILURE;
    }
    if(status != OS_OK)
        LOGD("OS_TimerStop: FAILED\n");

    return (status);
}

/**
 * OS Timer Set function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @param
 *    ULONG ulPeriod - timer period
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerSet(OS_TIMER_ID ptimerId, ULONG ulPeriod)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerId;
    OS_STATUS status  = OS_OK;

    if (NULL != pTimer)
    {
        struct itimerspec itimer;

        /* configure timer
         * it_interval is zero for non-repeating timers */
        itimer.it_interval.tv_sec  = 0;
        itimer.it_interval.tv_nsec = 0;
        itimer.it_value.tv_sec     = ulPeriod/CLOCKS_PER_SEC;
        itimer.it_value.tv_nsec    = (ulPeriod%CLOCKS_PER_SEC)*1000;

        /* add the timer */
        if (timer_settime (pTimer->timerID, 0, &itimer, NULL) < 0)
        {
            status = OS_FAILURE;
        }
    }
    else
    {
        status = OS_FAILURE;
    }
    if(status != OS_OK)
        LOGD("OS_TimerSet: FAILED\n");

    return (status);
}

/**
 * OS Timer Set (msec) function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @param
 *    ULONG ulPeriod - timer period (in msec)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerSetMsec(OS_TIMER_ID ptimerId, ULONG ulPeriod)
{
    /* convert milliseconds to ticks */
    ulPeriod = (ulPeriod*(CLOCKS_PER_SEC/1000));

    /* schedule the timer */
    return (OS_TimerSet(ptimerId, ulPeriod));
}

/**
 * OS Timer Set Repeat function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @param
 *    ULONG ulPeriod - timer period
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerSetRepeat(OS_TIMER_ID ptimerId, ULONG ulPeriod)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerId;
    OS_STATUS status  = OS_OK;

    if (NULL != pTimer)
    {
        struct itimerspec itimer;

        /* configure timer
         * it_interval is zero for non-repeating timers */
        itimer.it_interval.tv_sec  = ulPeriod/CLOCKS_PER_SEC;;
        itimer.it_interval.tv_nsec = (ulPeriod%CLOCKS_PER_SEC)*1000;
        itimer.it_value.tv_sec     = itimer.it_interval.tv_sec;
        itimer.it_value.tv_nsec    = itimer.it_interval.tv_nsec;

        /* add the timer */
        if (timer_settime (pTimer->timerID, 0, &itimer, NULL) < 0)
        {
            status = OS_FAILURE;
        }
    }
    else
    {
        status = OS_FAILURE;
    }
    
    if(status != OS_OK)
        LOGD("OS_TimerDelete: FAILED\n");

    return (status);
}

/**
 * OS Timer Set Repeat (msec) function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @param
 *    ULONG ulPeriod - timer period (in msec)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS  OS_TimerSetRepeatMsec(OS_TIMER_ID ptimerId, ULONG ulPeriod)
{
    /* convert milliseconds to ticks */
    ulPeriod = (ulPeriod*(CLOCKS_PER_SEC/1000));

    /* schedule the timer */
    return (OS_TimerSetRepeat(ptimerId, ulPeriod));
}

OS_STATUS OS_TimerGetTime(OS_TIMER_ID ptimerID, ULONG *pulTimeRemaining)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerID;
    OS_STATUS status  = OS_OK;

    if ( (NULL != pTimer) && (NULL != pulTimeRemaining) )
    {
        struct itimerspec itimer;

        if (timer_gettime (pTimer->timerID, &itimer) < 0)
        {
            status = OS_FAILURE;
        }
        else
        {
            *pulTimeRemaining = ((itimer.it_value.tv_sec * CLOCKS_PER_SEC) + (itimer.it_value.tv_nsec / 1000));
        }
    }
    else
    {
        status = OS_FAILURE;
    }

    return (status);
}

/**
 * OS Timer Get Time (msec) function
 *
 * @param
 *    OS_TIME_ID *ptimerId - pointer to an timer identifier
 *
 * @param
 *    ULONG *pulTimeRemaining - pointer to time remaining (in msec)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerGetTimeMsec(OS_TIMER_ID ptimerID, ULONG *pulTimeRemaining)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerID;
    OS_STATUS status  = OS_OK;

    if ( (NULL != pTimer) && (NULL != pulTimeRemaining) )
    {
        struct itimerspec itimer;

        if (timer_gettime (pTimer->timerID, &itimer) < 0)
        {
            status = OS_FAILURE;
        }
        else
        {
            /* calculate time and convert to milliseconds */
            *pulTimeRemaining = (itimer.it_value.tv_sec * 1000) + (itimer.it_value.tv_nsec / 1000000);
        }
    }
    else
    {
        status = OS_FAILURE;
    }

    return (status);
}

/**
 * Get Timer Func function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @retval
 *    OS_FUNCPTR func - timer function,if successful
 *    NULL - if not successful
 */
OS_FUNCPTR GetTimerFunc(OS_TIMER_ID ptimerId)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerId;

    if (NULL != pTimer)
    {
        return ((OS_FUNCPTR)pTimer->pvFunction);
    }
    else
    {
        return (OS_FUNCPTR)NULL;
    }
}

ULONG OS_GetTickRate(void)
{
    return (CLOCKS_PER_SEC);
}

ULONG OS_GetTicks(void)
{
    struct timeval  tp;
    gettimeofday(&tp, NULL);
    return ((tp.tv_sec * CLOCKS_PER_SEC) + tp.tv_usec);
}


/******************************************************************************
*******************************************************************************
**                                                                           **
**  I/O function prototypes                                                  **
**                                                                           **
*******************************************************************************
******************************************************************************/

/**
 * OS Open function
 *
 * @param
 *    char *strPath - file path
 *
 * @param
 *    int iOFlags - I/O flags
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_Open(char *strPath, int iOFlags)
{
    return OS_OK;
}

/**
 * OS Close function
 *
 * @param
 *    ULONG ulFileDes - file descriptor
 *
 * @retval
 *    None.
 */
void OS_Close(ULONG ulFileDes)
{
}

/**
 * OS Device Add function
 *
 * @param
 *    OS_DEV_HDR *pdevHdr - pointer to a device header
 *
 * @param
 *    char *name - device name
 *
 * @param
 *    int iDrvnum - drive number
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_iosDevAdd(OS_DEV_HDR *pdevHdr, char *name, int iDrvnum)
{
    return OS_OK;
}

/**
 * OS Device Install function
 *
 * @param
 *    OS_FUNCPTR funcCreate - create function
 *
 * @param
 *    OS_FUNCPTR funcDelete - delete function
 *
 * @param
 *    OS_FUNCPTR funcOpen - open function
 *
 * @param
 *    OS_FUNCPTR funcClose - close function
 *
 * @param
 *    OS_FUNCPTR funcRead - read function
 *
 * @param
 *    OS_FUNCPTR funcWrite - write function
 *
 * @param
 *    OS_FUNCPTR funcIOCTL - IOCTL function
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_iosDrvInstall(OS_FUNCPTR funcCreate,
                           OS_FUNCPTR funcDelete,
                           OS_FUNCPTR funcOpen,
                           OS_FUNCPTR funcClose,
                           OS_FUNCPTR funcRead,
                           OS_FUNCPTR funcWrite,
                           OS_FUNCPTR funcIOCTL
                           )
{
    return OS_OK;
}

/**
 * OS Interrupt Lock function
 *
 * @param
 *    None.
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_IntLock(void)
{
    return OS_OK;
}

/**
 * OS Interrupt Unlock function
 *
 * @param
 *    int iLockKey - key used to unlock
 *
 * @retval
 *    None.
 */
void OS_IntUnlock(int iLockKey)
{
}


/*
 * PRIVATE FUNCTIONS
 *****************************************************************************/

static PVOID linuxThread(PVOID param)
{
    OS_TASK *pthreadRecord = (OS_TASK*)param;

    if (NULL != pthreadRecord)
    {
        if (NULL != pthreadRecord->pvParam)
        {
            ( (OS_FUNCPTR_PARAM)(pthreadRecord->pvFunction) )(pthreadRecord->pvParam);
        }
        else
        {
            ( (OS_FUNCPTR)(pthreadRecord->pvFunction) )();
        }
    }

    pthread_exit(0);
    return 0;
}

static void linuxTimer(union sigval value)
{
    OS_TIMER *pTimer = (OS_TIMER *)value.sival_ptr;

    if (NULL != pTimer)
    {
        if (NULL != pTimer->pvParam)
        {
            ( (OS_FUNCPTR_PARAM)(pTimer->pvFunction) )(pTimer->pvParam);
        }
        else
        {
            ( (OS_FUNCPTR)(pTimer->pvFunction) )();
        }
    }
}
/*
#   undef OS_MsgQReceiveDbg
#   undef OS_MsgQRemoveDbg
#   undef OS_SemDeleteDbg
#   undef OS_SemTakeDbg
#   undef OS_SemGiveDbg
#   undef OS_MsgQReceive
#   undef OS_MsgQRemove
#   undef OS_SemDelete
#   undef OS_SemTake
#   undef OS_SemGive

extern "C" {
#if defined(NDEBUG)
    // Define depleted functions OS_XxxxDbg(...) to prevent link errors with differently compiled external callers.
    OS_STATUS OS_MsgQReceiveDbg(OS_MSG_Q_ID a, char *b, ULONG c, int d, char *e, int f)   { return OS_MsgQReceive(a,b,c,d); }
    OS_STATUS OS_MsgQRemoveDbg(OS_MSG_Q_ID a, char *b, ULONG c, char *d, int e)           { return OS_MsgQRemove(a,b,c); }
    OS_STATUS OS_SemDeleteDbg(OS_SEM_ID a, char *b, int c)                                { return OS_SemDelete(a); }
    OS_STATUS OS_SemTakeDbg(OS_SEM_ID a, int b, char *c, int d)                           { return OS_SemTake(a,b); }
    OS_STATUS OS_SemGiveDbg(OS_SEM_ID a, char *b, int c)                                  { return OS_SemGive(a); }

#else

    // Define depleted functions OS_Xxxx(...) to prevent link errors with differently compiled external callers.
    OS_STATUS OS_MsgQReceive(OS_MSG_Q_ID a, char *b, ULONG c, int d) { return OS_MsgQReceiveDbg(a,b,c,d,"???",0); }
    OS_STATUS OS_MsgQRemove(OS_MSG_Q_ID a, char *b, ULONG c)         { return OS_MsgQRemoveDbg(a,b,c,"???", 0); }
    OS_STATUS OS_SemDelete(OS_SEM_ID a)                              { return OS_SemDeleteDbg(a,"???",0); }
    OS_STATUS OS_SemTake(OS_SEM_ID a, int b)                         { return OS_SemTakeDbg(a,b,"???",0); }
    OS_STATUS OS_SemGive(OS_SEM_ID a)                                { return OS_SemGiveDbg(a,"???",0); }

#endif
}*/


