#ifndef BLURAY_H_
#define BLURAY_H_
extern "C"
{
#include <stdint.h>
#include <unistd.h>

#include "file.h"
#include "logging.h"
#include "navigation.h"
#include "osapi.h"
}

#include "IndexTable.h"
#include "MovObject.h"
#include "Sound.h"
#include "BlurayRegisters.h"
#include "BlurayPlayController.h"
#include "HdmvMode.h"
#include "BDJMode.h"
#include "UOController.h"
#include <pthread.h> 


namespace RKBluray {


#define MSG_QUEUE_DEPTH 5

#define MEDIA_BLURAY 300

#define OPERATION_SUCC 0
#define OPERATION_FAIL 1

enum BlurayOperaion
{
    //////////////////////////////////add by hh@rock-chips.com for bluray///////////////////////
    KEY_PARAMETER_BLURAY_GET_VIDEO_INFOR = 7000,
    KEY_PARAMETER_BLURAY_GET_AUDIO_INFOR = 7001,
    KEY_PARAMETER_BLURAY_GET_SUBTITLE_INFOR = 7002,
    
    // get Audio/Subtitle track
    KEY_PARAMETER_BLURAY_GET_AUDIO_TRACK = 7003,
    KEY_PARAMETER_BLURAY_GET_SUBTITLE_TRACK = 7004,
    
    // set Audio/Subtitle track
    KEY_PARAMETER_BLURAY_SET_AUDIO_TRACK = 7005,
    KEY_PARAMETER_BLURAY_SET_SUBTITLE_TRACK = 7006,
    
    // set/get IG show/hide
    KEY_PARAMETER_BLURAY_SET_IG_VISIBLE = 7007,
    KEY_PARAMETER_BLURAY_GET_IG_VISIBLE = 7008,
    
    // set/get subtitle show/hide
    KEY_PARAMETER_BLURAY_SET_SUBTITLE_VISIBLE = 7009,
    KEY_PARAMETER_BLURAY_GET_SUBTITLE_VISIBLE = 7010,
    
    // IG operation
    KEY_PARAMETER_BLURAY_SET_IG_OPERAION = 7011,
    
    // chapter operation
    KEY_PARAMETER_BLURAY_GET_CHAPTER = 7012,
    KEY_PARAMETER_BLURAY_PLAY_CHAPTER = 7013,
    KEY_PARAMETER_BLURAY_GET_CURRENT_CHAPTER = 7014,

    // Angle opration
    KEY_PARAMETER_BLURAY_GET_NUMBER_OF_ANGLE = 7015,
    KEY_PARAMETER_BLURAY_GET_CURRENT_ANGLE = 7016,
    KEY_PARAMETER_BLURAY_PLAY_ANGLE = 7017,

    // Title Operation
    KEY_PARAMETER_BLURAY_GET_NUMBER_OF_TITLE = 7018,
    KEY_PARAMETER_BLURAY_GET_CURRENT_TITLE = 7019,
    KEY_PARAMETER_BLURAY_PLAY_TITLE = 7020,
    KEY_PARAMETER_BLURAY_PLAY_TOP_TITLE = 7021,

    KEY_PARAMETER_BLURAY_PLAY_NEXT_CONTENT = 7022,

    // Time seek
    KEY_PAEAMETER_BLUERAYP_PLAY_SEEK_TIME = 7023,
};

typedef enum tagMSG_TYPE
{
    MSG_EXIT = 0,
    MSG_PROCESS_USER_EVENT,
    MSG_PROCESS_HDMV_EVENT,
    MSG_PROCESS_BDJ_EVENT,
    MSG_DISC_INSERT,
    MSG_PLAY_MAIN_TITLE,
    MSG_PLAY_BUTTON_SOUND,
    MSG_DISC_ERROR,

    /* All new message types above this line */
    MSG_INVALID
} MSG_TYPE;

typedef enum tagCOMMAND
{
    EXIT = 0,    // 0
    MENUCALL,    // 1
    TITLESEARCH, // 2
    CHAPTERSEARCH,
    TIMESEARCH,
    SKIPTONEXTPOINT, // 5
    SKIPTOPREVPOINT,
    RESTARTPOINT,
    PLAY,
    PLAYLOCATION,
    STOP,  //10
    PAUSEON,
    PAUSEOFF,
    STILLOFF,
    FORWARDPLAY,
    BACKWARDPLAY, // 15
    RESUME,
    MOVEUP_SELECTED_BUTTON,
    MOVEDOWN_SELECTED_BUTTON,
    MOVELEFT_SELECTED_BUTTON,
    MOVERIGHT_SELECTED_BUTTON, // 20
    MOVEUP_SELECTED_BUTTON_RELEASE,
    MOVEDOWN_SELECTED_BUTTON_RELEASE,
    MOVELEFT_SELECTED_BUTTON_RELEASE,
    MOVERIGHT_SELECTED_BUTTON_RELEASE,
    SELECTBUTTON,
    ACTIVATEBUTTON,
    SELECTANDACTIVATEBUTTON,
    AUDIOCHANGE,
    PGANDSTCHANGE,
    PGANDSTENABLE,
    TEXTSUBTITLESTYLECHANGE,
    ANGLECHANGE,
    POPUPON,
    POPUPOFF,
    REPEAT,
    COLOR_KEY,
    JUMPOBJ,
    CALLOBJ,
    JUMPTITLE,
    CALLTITLE,
    BDJ_TITLECHANGE,
    HDMV_TERMINATED,
    BDJ_TERMINATED,
    TITLE_STARTED,
    BOB_COMPELETE,
    INVALID_COMMAND
}COMMAND;

typedef struct tagBLURAY_MESSAGE
{
    MSG_TYPE            mMsgType;
    COMMAND             cmd;
    int64_t             data;
    int                 reson;
    OS_SEM_ID           sem;
} BlURAY_MESSAGE;

typedef struct bd_chapter {
    uint32_t    idx;
    uint64_t    start;
    uint64_t    duration;
    uint64_t    offset;
} BD_TITLE_CHAPTER;

typedef struct bd_title_info {
    uint32_t    idx;
    uint32_t    angle_count;
    uint32_t    chapter_count;
    uint64_t    duration;
    BD_TITLE_CHAPTER *chapters;
} BD_TITLE_INFO;

/**
 * Module Manager Title Type
 */
typedef enum tagTITLE
{
    TITLE_NONE = 0,
    TITLE_HDMV_MOVIE,
    TITLE_HDMV_INTERACTIVE,
    TITLE_BDJ_MOVIE,
    TITLE_BDJ_INTERACTIVE
} TITLE;

typedef struct tagOBJECT
{
    bool             mValid;
    TITLE            mTitleType;
    INDEX_OBJ_ID     mObjID;
    unsigned long    mSuspendedCmdID;

    tagOBJECT()
    {
        mValid = false;
        mTitleType = TITLE_NONE;
        mSuspendedCmdID = -1;
    }
} OBJECT;

/**
 * Module Manager State
 */
typedef enum tagSTATE
{
    STATE_TERMINATED_FIRSTPLAY = 0,
    STATE_TERMINATED,
    STATE_RUNNING,
    STATE_HOLD
} STATE;

class Bluray :public PlayControlEventCallback,public HDMVEventCallBack 
{
private:
    char *mPath;

    IndexTable* mIndex;
    MovieObject* mMovieObject;
    Sound* mSound;
    BlurayRegisters* mRegister;
    BlurayPlayController* mPlayController;
    UOController * mUOCtrl;
    BlurayPlayCallBack*  mPlayBack;
//    RKPlayerConfig* mRKPlayerConfig;

    int               mTaskId;
    OS_MSG_Q_ID         mMsgQID;
    HDMV* mHDMV;
    BDJMode* mBDJ;

    int mState;
    int mMediaPlayerState;

    OBJECT mActive;                           // excute object current
    OBJECT mSuspend;                            // suspend object
    int mCurrentTitle;                        // 当前播放Title的类型

    bool mBDJTitleRepeat;
 //   static int Number;
    pthread_mutex_t mMutex;
public:    
    Bluray(const char* path);
    virtual ~Bluray();
    TextFont* getTextSubtitleFont(int index);
    char* getBasePath();
    
    int getTrackCount(int type);
    unsigned int getDuration();

    int getCurrentTextSubtitleStyle();
    int getCurrentAudio();
    int getCurrentSubtitle();
    int getCurrentTitle();
    
    int getNumberOfTitle();
    void playMainTitle();
    bool isBDDirectory();
    BlurayPlayController* getPlayController();
    void setPlayCallBack(BlurayPlayCallBack* callback);
    IGSController* getIGSController();
    void playButtonSoundEffect(SoundData* data,SOUND_ATTRIBUTES* attribute);
    int addUserEvent(int cmd,int64_t param);
    int getRepeatMode();
    unsigned int getTitleDuration();
    int getNumberOfChapters();
    int getNumberOfAngles();
    int getCurrentAngle();
    MPLS_STREAM* getTrackInfor(int type);
    int playComplete();
    int playNext();
    void blurayPlay();//BDOpen(JavaVM *vm);
    int igsActivateButton(IGCommand* cmd,int number);
    int igsPlayButtonSound(int soundId);
    void eventCallBack(int event,unsigned long param);
    int sendHdmvEvent(COMMAND cmd,int data);
    int HdmvUninitialize();
    int jumpMovieObject(int id);
    int callTitle(int title);
    int callObject(int id);
    int jumpTitle(int title);
    int resume();
    int HdmvTerminated(void);

    // user operation methods
    int UOP_Resume();
    int UOP_MenuCall();
    int UOP_Play();
    int UOP_Stop(bool forceStop);
    int UOP_TitleSearch(int title);
    int UOP_SetRepeat(bool on);
    
    bool userCommandIsValid(int cmd);
    int userEventProcess(int cmd,int64_t param);
    static ULONG BlurayTaskThread(void* params);
};

}
#endif
