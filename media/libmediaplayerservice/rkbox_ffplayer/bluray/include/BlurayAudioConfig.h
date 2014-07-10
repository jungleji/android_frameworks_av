#ifndef _RK_BLURAY_AUDIO_H_
#define _RK_BLURAY_AUDIO_H_

/*
* define by hh@rock-chips.com for RK Blyray playing
*/

#include "BlurayConfig.h"


namespace RKBluray {



// The Audio Code Type 编码
#define Audio_Code_MPEG1  0x03
#define Audio_Code_MPEG2  0x04
#define Audio_Code_LPCM   0x80
#define Audio_Code_AC3    0x81
#define Audio_Code_DTS    0x82
#define Audio_Code_AC3_TRUE_HD  0x83
#define Audio_Code_AC3_PLUS     0x84
#define Audio_Code_DTS_HD       0x85
#define Audio_Code_DTS_HD_MASTER  0x86
#define Audio_Code_AC3_PLUS_SECONDARY  0xa1
#define Audio_Code_DTS_HD_SECONDARY    0xa2

// The Audio Sample Freq 采样率
#define Audio_Sample_48k   0x01
#define Audio_Sample_96k   0x04
#define Audio_Sample_192k  0x05
#define Audio_Sample_48_192k 0x0C
#define Audio_Sample_48_96k  0x0E

// The Audio Channel
#define Audio_Channel_Mono    0x01
#define Audio_Channel_Stereo  0x03
#define Audio_Channel_Multichannel  0x06
#define Audio_Channel_Stereo_Plus_Multichannel  0x0C


#if 0
struct BlurayAudioConfig
{
    int mPlayItem;    
    int mPid;          
    int mSampleRate;     // 采样率
    int mChannel;  // 声道
    int mCode;   // 编码
    int repeat;

    long long mStartOffset;
    long long mEndOffset;

    char*  mPath;
    char   mLanguage[4];

    BlurayAudioConfig()
    {
        mPlayItem = 0;
        mPid = 0;
        mSampleRate = 0;
        mChannel = 0;
        mCode = 0;
        repeat = 0;

        mStartOffset = mEndOffset = 0;
        mPath = NULL;
        memset(mLanguage,0,sizeof(mLanguage));
    }


    ~BlurayAudioConfig()
    {
        if(mPath != NULL)
        {
            free(mPath);
            mPath = NULL;
        }
    }
};
#endif

class BlurayAudioConfig:public BlurayConfig
{
public:
    int mSampleRate;     // 采样率
    int mChannel;  // 声道
    int repeat;
    char mLanguage[4];

    BlurayAudioConfig()
    {
        mSampleRate = 0;
        mChannel = 0;
        repeat = 0;
        memset(mLanguage,0,sizeof(mLanguage));
    }
};


}
#endif

