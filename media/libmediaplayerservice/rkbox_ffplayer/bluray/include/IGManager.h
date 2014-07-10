#ifndef _RK_BLURAY_IG_
#define _RK_BLURAY_IG_

#include <stdio.h>
#include "vcitypes.h"
#include "BlurayRegisters.h"
#include "NavigationCommand.h"
#include <pthread.h> 
#include "LOGD.h"
#include "BlurayIGConfig.h"

extern "C"
{
#include "vcitypes.h"
#include "osapi.h"
#include "colorspace.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavcodec/ig.h"
#include "libavformat/mpegts.h"
}
#include <utils/Vector.h>


using namespace android;




namespace RKBluray 
{

#define AV_NUM_DATA_POINTERS 8
#define AVPALETTE_SIZE 1024

#define MAX_BUTTONID_VALUE   0x1FDF
#define MAX_BUTTON_NUMBER_VALUE         0x270F /* '9999' */


#define NUMBER 256 
#define PICTURENUMBER (4096)



/* macros */
#define SWITCH_1(byte)                  (0x80 & byte)
#define SWITCH_2(byte)                  (0x40 & byte)
#define RLZERO(byte)                    (0x3f & byte)
#define RL3TO63(byte)                   (0x3f & byte)
#define RL64TO16K_TOP(byte)             (0x3f & byte)


/**
 * Message
 */
typedef struct tagIGManagerMessage
{
    int                 tMsgType;
    ULONG               ulData0;
    ULONG               ulData1;
} IGManagerMessage;//IGSPARSE_MESSAGE;

typedef enum IGS_MANAGER_MSG_TYPE
{
    IGS_MANAGER_MSG_PARSER = 0,
    IGS_MANAGER_MSG_EXIT,
} IGS_MANAGER_MSG_TYPE;



typedef enum IGS_SELECT_BUTTON_DIRECTION
{
    SelectButton_MoveUp = 0,
    SelectButton_MoveDown ,
    SelectButton_MoveLeft ,
    SelectButton_MoveRight ,
    SelectButton_Activate,
    IGS_ON_OFF,
}IGS_SELECT_BUTTON_DIRECTION;


/**
 * four components are given, that's all.
 * the last component is alpha
 */
typedef struct AVPicture {
    unsigned char *data[AV_NUM_DATA_POINTERS];
    int linesize[AV_NUM_DATA_POINTERS];     ///< number of bytes per line
    unsigned int* pic;

    AVPicture()
    {
        reset();
    }

    ~AVPicture()
    {
        freeAVPicture();
    }

    void reset()
    {
        for(int i = 0; i < AV_NUM_DATA_POINTERS; i++)
        {
            data[i] = NULL;
            linesize[i] = 0;
        }
    }

    void freeAVPicture()
    {
        for(int i = 0;i < AV_NUM_DATA_POINTERS; i++)
        {
            linesize[i] = 0;
            data[i] = NULL;
        }
    }
} AVPicture;

typedef struct AVRect {
    int x;         ///< top left corner  of pict, undefined when pict is not set
    int y;         ///< top left corner  of pict, undefined when pict is not set
    int w;         ///< width            of pict, undefined when pict is not set
    int h;         ///< height           of pict, undefined when pict is not set
    int nb_colors; ///< number of colors in pict, undefined when pict is not set

    /**
     * data+linesize for the bitmap of this overlay.
     */
    AVPicture pict;

    AVRect()
    {
        x = 0;
        y = 0;
        w = 0;
        h = 0;
        nb_colors = 0;
        pict.reset();
    }

    ~AVRect()
    {
        pict.freeAVPicture();
    }
} AVRect;

typedef struct AVIGS {
    static int mHasAnimation ;  
    unsigned int num_rects;
    unsigned int format; /* 0 = HDMV Interactive Graphic menu */
    unsigned int start_display_time; /* relative to packet pts, in ms */
    unsigned int end_display_time; /* relative to packet pts, in ms */
    
    int64 pts;    ///< Same as packet pts, in AV_TIME_BASE
    AVRect **rects;

    AVIGS()
    {
        format = 0;
 //       mHasAnimation = 0;
        start_display_time = 0;
        end_display_time = 0;
        num_rects = 0;
        pts = 0;
        rects = NULL;
    }

    ~AVIGS()
    {
        if(rects != NULL)
        {
            for(int i = 0; i < num_rects; i++)
            {
                if(rects[i] != NULL)
                {
                    delete rects[i];
                    rects[i] = NULL;
                }
            }
            delete[] rects;
            rects = NULL;
        }

        num_rects = 0;
    }
} AVIGS;

class IGSShowCallBack
{
public:
    virtual void igsSelectedButtonCmdAutoRun() = 0;
    virtual void igsPushDataToShow(AVIGS* data) = 0;

    virtual int igsShowCallback(AVIGS* igs) = 0;
    virtual void IGParseOver() = 0;
    virtual void clearIGSSurface() = 0;
    virtual ~IGSShowCallBack(){};
};

class IGDisplayObject
{
public:
    IGDisplayObject()
    {
        mPage = 0;
        mDirty = 0;
        mXPosition = 0;
        mYPosition = 0;
        mWidth = 0;
        mHeight = 0;
        mButtonID = 0;
        mBOGID = 0;
        mPictureID = 0;
        mData = NULL;
    }
public:
    int              mPage;
    int              mDirty;
    int              mXPosition;
    int              mYPosition;
    int              mWidth;
    int              mHeight;
    unsigned int     mButtonID;
    unsigned int     mBOGID;
    unsigned int     mPictureID;
    unsigned char*   mData;
};

class IGManager
{
public:
    IGManager();
    ~IGManager();
    void            resetButtonStatus();
    void            initIG(IG* ig);
    void            deleteIG(IG* ig);
    int				readPacket();
    static void*  readIGPacketThread(void * data);
    int load(Vector<BlurayIGConfig*>* list);
    void closeIGContext();
    int initIGContext();
    int initIGTrack(AVFormatContext *ic);
    int initIGDecoder(AVFormatContext *ic);
    
    int sendMsgParseIG(Vector<BlurayIGConfig*>* list);
    IG* getIG();
    int decodeRle(IGSPicture* pic,unsigned char* outBuf);
    int processPicture(IG* ig);
//    void freeIG(IG* ig);
    int parse();
    bool isOutMux();
    bool isPopUpMenu();
    bool isDisplayEnable();
    void setPopupEnable(bool enable);
    int findPageAndSelectedButton();
    int findFirstSelectedButton();
    int findSelectedButton(IGSPage* page,int selectedId,IGSBOG** buttonGroup,int* bogIndex,IGSButton** button);
    int disableBogButton(IGSBOG* bog,int buttonId);
    int disableButton(int buttonId);
    int getBogIndex(IGSPage*page, int id);
    int disableBog(IGSBOG* bog,int id);
    int buttonActivateComplete();
    int enableButton(int buttonId);
    int findBOGDefaultButton(IGSPage* page,IGSBOG** buttonGroup,int* bogIndex,IGSButton** button);
    int findButton(IGSPage* page,int buttonId,IGSButton** button);
    int findButton(IGSPage* page,IGSBOG** bog,int* bogIndex,IGSButton** button);
    int findButton(IGSBOG** buttonGroup,IGSButton** button);
    int findAnyValidButton(IGSPage* page,IGSButton** button);
    void setBOGDefaultButton(IGSPage* page,int buttonId);
    AVIGS* moveSelectedButton(int direct);
    int setPage(int id);
    int setNextPage(int id);
    int getPage();
    int getButtonId(IGSPage* page,int index);
    int selectButton(int buttonId);
    int selectNextButton(int buttonId);
    IGSButton* getSelectedButton();
    int activeSelectButton(bool active);
    
 //   int display(int a);
    AVIGS* display();
    void setCallBack(IGSShowCallBack* callback);
    void setRegister(BlurayRegisters * reg);
    static ULONG IGSParseThread(void* data);
public:
    IG*   mIg;
    AVFormatContext *mIGContext;
    IGSButton*   mSelectedButton;
    IGSButton*   mNextSelectedButton;
    IGSButton*   mHintButton;
    
    int          mPage;  // current show page
    int          mNextPage;
    int          mPageInDisplayObject;

    int          mHasAnimation;
    int                      mIgTrack;
    Vector<BlurayIGConfig*>* mIgList;

 //   Vector<IGDisplayObject*>*      mDisplayList;

    BlurayRegisters* mRegister;
    IGSShowCallBack* mCallBack;

    bool         mCommandExcuteCompelete;
    bool         mPopupEnable;

    bool         mExit;

    pthread_t                   mIGSParseThread;

    int                         mIGParseThreadStatus;

    pthread_mutex_t mParseMutex;
};

}
#endif
