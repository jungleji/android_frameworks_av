#ifndef RK_BLURAY_TEXTSUBTITLE_CONTROLLER_H_
#define RK_BLURAY_TEXTSUBTITLE_CONTROLLER_H_

#include "BlurayRegisters.h"

#include "IGSParser.h"
//#include "IGSRender.h"
#include "IGSDisplayCallBack.h"
#include "utils/Vector.h"
//#include "CCImage.h"
#include "PlayControlEventCallback.h"
#include "BlurayCallBack.h"
#include "SubtitleParser.h"
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include "Subtitle.h"


extern "C"
{
#include "vcitypes.h"
#include "osapi.h"
}


using namespace android;


namespace RKBluray
{

typedef struct tagtSubtitle_MESSAGE
{
    int                 tMsgType;
    ULONG               ulData0;
    ULONG               ulData1;
    PVOID               pvBuffer;
    ULONG               ulBufferLength;
    ULONG               ulSemID;
    ULONG*              pulRetVal;
} Subtitle_MESSAGE;


typedef enum SubtitleParse_MSG_TYPE
{
    SubtitleParse_MSG_SUBTITLE_PARSER = 0,
    SubtitleParse_MSG_FONT_PARSER = 1,
    SubtitleParse_MSG_EXIT
} SubtitleParse_MSG_TYPE;


class SubtitleController
{
public:
    SubtitleController(char* path);
    ~SubtitleController();
    void SubtitleTurnOn();
    void SubtitleTurnOff();
    int parse(char* path,int pid);
    int display(sp<Surface> surface,Subtitle* subtitle);
    int display(Subtitle* subtitle);
    int exit();
    int display_RGA(void* data,int width,int height);
    int parseTextSubtitleFont(char* path);
    static ULONG SubtitleDisplayThread(void* data);
private:
    int mUserStyleId;
    int mSurfaceWidth;
    int mSurfaceHeight; 
    int mRGAFd;
    
    
    SubtitleParser* mParser;
    sp<SurfaceComposerClient> mClient;
    sp<Surface> mSurface;
    sp<SurfaceControl> mControl;

    

    bool mSubtitleVisible;
    OS_MSG_Q_ID  mSubtitleDisplayMsgQID;
    unsigned int mSubtitleDisplayThread;
    
};


}
#endif
