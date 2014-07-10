#ifndef _IGS_CONTROLLER_
#define _IGS_CONTROLLER_
/*
* this file is used to control the IGS
*
* write by hh@rock-chips.com
* 2013-06-21
*
*/

#include "BlurayRegisters.h"

#include "IGSDisplayCallBack.h"
#include <utils/Vector.h>
#include "PlayControlEventCallback.h"
#include "BlurayCallBack.h"
#include "BlurayIGConfig.h"
#include "IGManager.h"

extern "C"
{
#include "vcitypes.h"
#include "osapi.h"

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavcodec/ig.h"
#include "libavformat/mpegts.h"
}


#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/Surface.h>


#include <utils/Vector.h>
#include <utils/List.h>

#include "IonAlloc.h"


#define IGSShow 0
#define IGSHide 1

#ifndef INT64_C
#define INT64_C
#define UINT64_C 
#endif


#define ASSERT_TRUE(condition)  ;
#define ASSERT_EQ ;
using namespace android;

namespace RKBluray
{

#define ION_MEMO (1920*1080*4)

typedef enum COLOR_CONFIG
{
    ARGB_888_Config = 0,
    RGB_565_Config
} COLOR_CONFIG;

/**
 * Message
 */
typedef struct tagIGSCONTROLL_MESSAGE
{
    int                 tMsgType;
    ULONG               ulData0;
    ULONG               ulData1;
    PVOID               pvBuffer;
    ULONG               ulBufferLength;
    ULONG               ulSemID;
    ULONG*              pulRetVal;
} IGSCONTROLL_MESSAGE;

typedef enum IGS_MSG_TYPE
{
    IGS_MSG_PARSER = 0,
    IGS_MSG_CLEAR,
    IGS_MSD_JUST_SHOW,
    IGS_MSD_SHOW,
    IGS_MSD_MOVE_BUTTON,
    IGS_MSG_ACTIVATE_BUTTON,
    IGS_MSG_TURN_PAGE,
    IGS_MSG_BOB_COMPELETE,
    IGS_MSG_EXIT,
 
} IGS_MSG_TYPE;


class IGSController:public IGSShowCallBack
{
public:
        IGSController(BlurayRegisters* reg);
        ~IGSController();
        
        int parse(BlurayIGConfig* config);
        int sendParseMessage(Vector<BlurayIGConfig*>* list);
        int initSurface();
#ifdef AVS44      
        int display_RGA(ANativeWindow_Buffer& nativeWindow,AVIGS* igs);
#else
        int display_RGA(Surface::SurfaceInfo& surfaceInfor,AVIGS* igs);
#endif
        void surfaceHide();
        void surfaceShow();
        void clearIGS();
        void setPlayEventCallBack(PlayControlEventCallback* playCallback);
        void load(Vector<BlurayIGConfig*>* list);
        int parserReset();
        int exit();
        void displayIGS();
        void igsSelectedButtonCmdAutoRun();
        void igsPushDataToShow(AVIGS* data);
        bool getSurfaceVisibility();
        void setSurfaceVisibility(bool visible);
        bool isPopupMenu();
        int popOnMenu();
        int popOffMenu();
        int enbaleButton(int buttonId);
        int disableButton(int buttonId);
        int sendMsgTodisplayIGS();
        int selectButtonAndPage(unsigned int buttonId,unsigned int pageId,int flag);
        int selectButton(int buttonNumber);
        int moveSelectButton(int direction);
        int activateButton();
        void clearIGSSurface();
        int igsCommandCompelete();
        int IGSStartAnimation();

        void IGParseOver();
        int igsShowCallback(AVIGS* igs);
        
        static ULONG timerDisplayCallback(void* param);
        static ULONG IGSDisplayThread(void* data);
    private:
        BlurayRegisters* mRegister;
        PlayControlEventCallback* mPlayCallBack;
        IGManager*  mIGManager;

        bool mIGSSurfaceVisible;
        bool mIsWriteSurface;

        int mSurfaceWidth;
        int mSurfaceHeight;

        int mRGAFd;

        OS_MSG_Q_ID  mIGSDisplayMsgQID;
        unsigned int mIGSDisplayThread;

        OS_TIMER_ID mAnimationTimer;     /* The animation timer */
    //    OS_TIMER_ID TimerUserSelection; /* User Selection timer */

#if(USE_ION)        
        IonAlloc*        mIon;
        alloc_data       mIonData;
        int              mIonStatus;
#endif
        sp<SurfaceComposerClient> mClient;
        sp<Surface> mSurface;
        sp<SurfaceControl> mControl;
#ifndef AVS44         
        Surface::SurfaceInfo mSurfaceInfo;
        uint8_t *mSurfaceBuf;
#endif
        Vector<AVIGS*>* mIgDisplayList;
        pthread_mutex_t mDisplayMutex;
        pthread_mutex_t mMutex;
};

}
#endif


