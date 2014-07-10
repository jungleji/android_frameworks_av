#ifndef _IGS_PARSE_H_
#define _IGS_PARSE_H_ 


#include <stdio.h>
#include "IGSDisplayCallBack.h"
#include "vcitypes.h"
#include "osapi.h"
#include "BlurayRegisters.h"
#include <pthread.h> 
#include "IG.h"
#include "LOGD.h"
#include "BlurayIGConfig.h"
#include <utils/Vector.h>

namespace RKBluray 
{

enum SegmentType {
    PALETTE_SEGMENT      = 0x14,
    PICTURE_SEGMENT      = 0x15,
    ICS_SEGMENT          = 0x18,
    DISPLAY_SEGMENT      = 0x80,
};


//#define BD_MPEG2_TS_SIZE 192
//#define MAX_PES_PACKAET 65535

#define MAX_WINDOW_NUM 2
#define MAX_EFFECT_NUM  128
#define MAX_EFFECT_OBJECT_NUM 2

using namespace android;


class IGSCallBack
{
public:
    virtual int igsShowCallback(AVIGS* igs) = 0;
    virtual void IGParseOver(IG*& ig) = 0;
    virtual void clearIGSSurface() = 0;

    virtual int igsShowCallback(AVIGS* igs) = 0;
    virtual void IGParseOver(IG*& ig) = 0;
    virtual void clearIGSSurface() = 0;
//    virtual void igsSelectedButtonCmdAutoRun() = 0;
    virtual ~IGSCallBack(){};
//    virtual void igsSelectedButtonCmdAutoRun() = 0;
    virtual ~IGSCallBack(){};
};


/**
 * Message
 */
typedef struct tagIGSPARSE_MESSAGE
{
    int                 tMsgType;
    ULONG               ulData0;
    ULONG               ulData1;
    PVOID               pvBuffer;
    ULONG               ulBufferLength;
    ULONG               ulSemID;
    ULONG*              pulRetVal;
} IGSPARSE_MESSAGE;

typedef enum IGS_PARSER_MSG_TYPE
{
    IGS_PARSER_MSG_PARSER = 0,
    IGS_PARSER_MSG_EXIT,
} IGS_PARSER_MSG_TYPE;

class IGSParser {
public:
    int             mOutSize;
    int             mPid;                      // ts pid
    unsigned long long mStartOffset;
    unsigned long long mEndOffset;

    unsigned long long mSize;

    int             mSurfaceWidth;
    int             mSurfaceHeight;
    float           mWidthScale;
    float           mHeightScale;

    int             mICSTempDataSize;
    int             mICSTempDataTotalSize;

    bool            mSoftScale;
    
    char*          mICSTempBuffer;

    IGSCallBack* mIGSParseCallback;
    BlurayRegisters* mRegister;
    FILE *mFile;
    char* mOutBuffer;
    IGSCallBack* mShowCallBack;

    OS_MSG_Q_ID mIGSParseMsgQID;
    unsigned int mIGSParseThread;

    IonAlloc*     mIon;

    
    int load(Vector<BlurayIGConfig*>* list);
    int initIGContext();
    int initIGTrack(AVFormatContext *ic);
    int initIGDecoder(AVFormatContext *ic);
    int parse();
    int readPacket();
    void* readIGPacketThread(void * data);
    
    int sendParseMessage(Vector<BlurayIGConfig*>* list);
    void setSurfaceWidthAndHeight(int width,int height);
//    IGSButton* getSelectedButton();
    void reset();
    void setPID(int pid);
    int findPESHead(char* buf, int len);
    // parse Mpeg2 ts
    int readMpeg2Ts(char* buf);
    int parseMpeg2Ts(char* buf,int size,IG* ig);
    int parsePESPacket(char* buffer,int inSize);
    int64 parsePtsDts(char*buf);
    int parseMpeg2TsAdapterFiled(char*& buffer);
    
    int decode(char *buf,IG* ig);
    int parseICS(IG* ig,char *buf, int buf_size);
    int parsePaletteSegment(IG* ig,char *buf, int buf_size);
    int parsePictureSegment(IG* ig,char *buf, int buf_size);
    int displayIGS(AVIGS* Igs);

    int displayEffectSequence(char*& p,EffectSequence *effectSequence);
    int displayWindow(char*& p);
    int displayEffect(char*& p);
    int displayCompositionObject(char*& p);
    
    void scalePicture(unsigned char* buffer,IGSPicture* pic);
    int processPicture(IG* ig);

    
    int IGRLEDecode(unsigned char *Input,int InputSize, unsigned char *Output, int OutputSize);
    int decodeRle(IGSPicture* pic,unsigned char* outBuf);
    int displayCmd(NavigationCommand* cmd);
    
    void printfbuf(char*  buf, int n);
    void setIGSParseCallBack(IGSCallBack* callback);
    int buttonActivateComplete();
    int parserExit();
    static ULONG IGSParseThread(void* data);
public:
    IGSParser(BlurayRegisters* blurayRegister,bool softscale,IonAlloc* ion);
    ~IGSParser();
    int parse(BlurayIGConfig*config,IG* ig);
    int getByte(char* buf);
    int getWORD(char* buf);
    int get3Byte(char* buf);
    int get4Byte(char* buf);

    
    pthread_mutex_t mParseMutex;
} ;

}
#endif
