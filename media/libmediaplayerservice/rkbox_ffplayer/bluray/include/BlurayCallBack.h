#ifndef _RK_BLURAY_CALLBACK_H_
#define _RK_BLURAY_CALLBACK_H_

#include <utils/Vector.h>
#include "BlurayVideoConfig.h"
#include "BluraySubtitleConfig.h"
#include "BlurayAudioConfig.h"
#include "BlurayIGConfig.h"
#include <utils/Vector.h>

using namespace android;

namespace RKBluray {

#define STOP_ALL 0
#define STOP_VIDEO 1
#define STOP_AUDIO 2
#define STOP_SUBTITLE 3
#define SEEK_PAUSE_ALL     4

#define PLAY_ALL 0
#define PLAY_VIDEO 1
#define PLAY_AUDIO 2
#define PLAY_SUBTITLE 3
#define SEEK_START_ALL 4


typedef enum BlurayMessage
{
    BDJ_NOT_SUPPORT = 0,
    USER_OPERATIONS_FORBIDDEN,
    Not_BLURAY_DISK,
};

class BlurayPlayCallBack
{
public:
    virtual void play(int type) = 0;
    virtual void stopPlay(int type) = 0;
    virtual void destory(int type) = 0;
    virtual void configVideo(bool configChange,int pid,Vector<BlurayVideoConfig*>* list,int begin) = 0;
    virtual void configAudio(bool configChange,int pid,Vector<BlurayAudioConfig*>* list,int begin) = 0;
    virtual void configSubtitle(bool configChange,int pid,Vector<BluraySubtitleConfig*>* list,int begin) = 0;
    virtual void videoSeek(int listPosition,int64_t offset,int64_t time) = 0;
    virtual void audioSeek(int listPosition,int64_t offset,int64_t time) = 0;
    virtual void subtitleSeek(int listPosition,int64_t offset,int64_t time) = 0;
    virtual int subtitleOnOff(int on) = 0;
    virtual void playButtonSound(int sample,int chanel,int bitPerSample,int length,unsigned char* soundData) = 0;
    virtual int textSubtitleStyleChange(int style) = 0;
    virtual int seekTimeTo(int msec) = 0;
    virtual void sendMsgToPlayer(int type,int code,int reson) = 0;
    virtual void sendMsg(int type,int code,int reson) = 0;
    virtual void setState(int status) = 0;
    virtual ~BlurayPlayCallBack(){};
};


}
#endif
