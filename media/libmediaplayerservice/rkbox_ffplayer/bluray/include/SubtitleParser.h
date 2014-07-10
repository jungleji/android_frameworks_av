#ifndef RK_BLURAY_TEXTSUBTITLE_PARSER_H_
#define RK_BLURAY_TEXTSUBTITLE_PARSER_H_

/*
* this file is added by hh@rock-chips for parsing Text Subtitle
*/


#include <stdio.h>
#include "Subtitle.h"
#include "BluraySubtitleConfig.h"
//#include <utils/Vector.h>
#include "BlurayCallBack.h"

extern "C"
{
#include "vcitypes.h"
#include "osapi.h"
}


namespace RKBluray
{
enum TextParseState
{
    State_INIT,
    State_PARSING,
    State_DEINIT,
};

typedef struct tagSubtitlePARSE_MESSAGE
{
    int                 tMsgType;
    ULONG               ulData0;
    ULONG               ulData1;
    PVOID               pvBuffer;
    ULONG               ulBufferLength;
    ULONG               ulSemID;
    ULONG*              pulRetVal;
} SubtitlePARSE_MESSAGE;


typedef enum {
    PGS_PALETTE        = 0x14,
    PGS_OBJECT         = 0x15,
    PGS_PG_COMPOSITION = 0x16,
    PGS_WINDOW         = 0x17,
    PGS_IG_COMPOSITION = 0x18,
    PGS_END_OF_DISPLAY = 0x80,
    /* Text subtitles */
    Text_DIALOG_STYLE        = 0x81,
    Text_DIALOG_PRESENTATION = 0x82,
} pgs_segment_type_e;

typedef enum Subtitle_PARSER_MSG_TYPE
{
    Subtitle_PARSER_MSG_PARSER = 0,
    Subtitle_PARSER_MSG_EXIT,
} Subtitle_PARSER_MSG_TYPE;



class SubtitleParser
{
public:
    SubtitleParser(char* path);
    ~SubtitleParser();
    
    int parse(BluraySubtitleConfig*config);
    void setState(int state);
    int getState();
    void setSubtitleCallBack(BlurayPlayCallBack* callback);
    int parseSubtitle(BluraySubtitleConfig* config);
private:
    #if 1
    char*     mPath;
    FILE*     mFile;
    char*     mOutBuffer;
    int       mOutSize;
    // stream id
    int       mPid;
    // the start position of file to parse
    unsigned long long     mStartOffset;
    // the end position of file to parse
    unsigned long long     mEndOffset;
    
    unsigned long long     mSize;
    #endif
    
    OS_MSG_Q_ID mSubtitleSParseMsgQID;
    unsigned int mSubtitleParseThread;
    
    int mState;

    BlurayPlayCallBack* mCallback;
    
    int exit();
//    int parse(BluraySubtitleConfig*config);
    int readMpeg2Ts(char* buf);
    int parseMpeg2Ts(char* buf,int size);
    int parsePESPacket(char* buffer,int inSize);
    int64 parsePtsDts(char* buf);
    int decode(char *buffer);
    int parseDialogStyle(char* buffer,int length,TextSubtitleDialogStyle* style);
    int parseDialogPresentation(char* buffer,int length,DialogPresentation* dialog);
    int decodeDialogStyle(char*& buffer,TextSubtitleDialogStyle* style);
    int decodeRegionStyle(char*& buffer,RegionStyle* style);
    int decodeUserStyle(char*& buffer,UserStyle* style);
    int decodePalette(char*& buffer,int count,PaletteEntry* palette);
    int decodeRegionInfo(char*& buffer,RectRegion* region);
    int decodeRect(char*& buffer,TextSubtitleRect* rect);
    int decodeFontSytle(char*& buffer,FontStyle* fontStyle);
    int decodeDialogPresention(char*& buffer,DialogPresentation* dialog);
    int decodeDialogRegion(char*& buffer,DialogRegion* region);
    static ULONG  SubtitleParseThread(void* data);
    unsigned char getByte(char* buf);
    int getWORD(char* buf);
    int get3Byte(char* buf);
    int get4Byte(char* buf);
};

    
}
#endif

