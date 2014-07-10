#ifndef _RK_BLURAY_HDMV_H_
#define _RK_BLURAY_HDMV_H_

#include "NavigationCommand.h"
#include "BlurayRegisters.h"
#include "NavigationCmdParser.h"
#include "IndexTable.h"
#include "MovObject.h"
#include "BlurayPlayController.h"
#include <pthread.h> 

//#include "control/BlurayPlayController.h"

extern "C"
{
#include <stdint.h>
#include <unistd.h>

#include "file.h"
#include "logging.h"
#include "navigation.h"
#include "osapi.h"
}


namespace RKBluray
{

typedef struct tagHDMV_MESSAGE
{
    int                 tMsgType;
    ULONG               ulData0;
    ULONG               ulData1;
    PVOID               pvBuffer;
    ULONG               ulBufferLength;
    ULONG               ulSemID;
    int*                pulRetVal;
} HDMV_MESSAGE;

/**
 * HDMV Module Message types
 */
typedef enum tagHDMVMOD_MSG_TYPE
{
    HDMVMOD_MSG_EXIT = 0,
    HDMVMOD_MSG_PROCESS_MOBJ,
    HDMVMOD_MSG_TERMINATE_MOBJ,
    HDMVMOD_MSG_SUSPEND_MOBJ,
    HDMVMOD_MSG_RESUME_MOBJ,
    HDMVMOD_MSG_LOAD_MOBJ,
    HDMVMOD_MSG_PROCESS_BOBJ,
    HDMVMOD_MSG_PLAYBACK_COMPLETE,
    HDMVMOD_MSG_NVTIMER_EXPIRE,
    HDMVMOD_MSG_REPEAT,
    HDMVMOD_MSG_PARSEIG,
    HDMVMOD_MSG_GOTO,

    /* All new message types above this line */
    HDMVMOD_MSG_INVALID
} HDMVMOD_MSG_TYPE;

/**
 * HDMV Callback Events
 */
typedef enum tagHDMV_EVENT_CODE
{
    HDMV_EVENT_MOBJ_COMPLETE = 0,
    HDMV_EVENT_RESUME_MOBJ,
    HDMV_EVENT_JUMP_MOBJ,
    HDMV_EVENT_CALL_MOBJ,
    HDMV_EVENT_JUMP_TITLE,
    HDMV_EVENT_CALL_TITLE,
    HDMV_EVENT_BUTTONEVENT_COMPLETE,
    HDMV_EVENT_INVALID
} HDMV_EVENT_CODE;

typedef enum tagHDMV_STATE
{
    HDMV_STATE_TERMINATED = 0,
    HDMV_STATE_RUNNING,
    HDMV_STATE_INVALID
} HDMV_STATE;



class HDMVEventCallBack
{
public:
    virtual void eventCallBack(int cmd,unsigned long param) = 0;
    virtual ~HDMVEventCallBack(){};
};

class HDMV
{
public:
    HDMV(char* path,BlurayRegisters* reg);
    HDMV(HDMV& hdmv);
    HDMV& operator = (const HDMV& hdmv);
    HDMV* operator = (const HDMV* hdmv);
    ~HDMV();
    
    void DeinitHDMV();
    void setPlayController(BlurayPlayController* play);
    int  getState();
    void setIndex(IndexTable* index);
    void setMovieObject(MovieObject* object);
    void setCallBack(HDMVEventCallBack* callBack);
    
    int playMovieObject(unsigned long id);
    int terminateMovieObject();
    int suspendMovieobject(unsigned long *cmdID);
    int resumeMovieObject(unsigned long  ulMobjID, unsigned long ulCmdID);
    int loadMovieObject(unsigned long ulMobjID, unsigned long ulCmdID);
//    int allocButtonObjectBuffer(NavigationCommand **ppButtonProgram, ULONG ulNumberOfCommands);
//    int freeButtonobjectBuffer(NavigationCommand *pButtonProgram);
    int processButtonObject(NavigationCommand *pButtonProgram, unsigned long ulNumberOfCommands);//, void* pContext
    int playbackCompelete();
    int navTimerExpire(void);
    int setTitleRepeat(bool on);
    bool isTitleRepeatOn(void);
    int getNavigationCommandID(void);
    int activateButtonComplete();
    void HdmvEventCallBack(int cmd,unsigned long param);
    static ULONG  HdmvModTask(void* pvParam);
private:
    BlurayRegisters* mRegister;
    char* mPath;
    MovieObject* mMovieObject;
    HDMVEventCallBack* mCallBack;

    NavigationCmdParser*  mNavigationCmdParser;
    unsigned long  mThread;
    OS_MSG_Q_ID  mMsgQID;
    OS_SEM_ID    mSem_sync;
    int mState;
    
    pthread_mutex_t mMutex;
    
    int threadExit();
};
    
}

#endif
