#ifndef _RK_BLURAY_VIDEO_H_
#define _RK_BLURAY_VIDEO_H_


#include "BlurayConfig.h"

namespace RKBluray {

// The Video Code Type 编码
#define VIDEO_Code_MPEG1  0x01
#define VIDEO_Code_MPEG2  0x02
#define VIDEO_Code_AVC    0xea
#define VIDEO_Code_VC1    0x1b
#define VIDEO_Code_MVC    0x20

// The Video Format 分辨率
#define VIDEO_Format_480i  1
#define VIDEO_Format_576i  2
#define VIDEO_Format_480p  3
#define VIDEO_Format_1080i 4
#define VIDEO_Format_720p  5
#define VIDEO_Format_1080p 6
#define VIDEO_Format_576p  7

// Frame Rate 刷新率
#define VIDEO_FrameRate_23  1   // 24000/1001 FPS
#define VIDEO_FrameRate_24  2   // 24 FPS
#define VIDEO_FrameRate_25  3   // 25 FPS
#define VIDEO_FrameRate_30  4   // 30000/1001 FPS
#define VIDEO_FrameRate_50  6   // 50 FPS
#define VIDEO_FrameRate_60  7   // 60000/1001 FPS

class BlurayVideoConfig:public BlurayConfig
{
public:
    int mFormat; // 1080p, 1080i..
    int mFrame;

    BlurayVideoConfig()
    {
        mFormat = 0;
        mFrame = 0;
    }
};


}
#endif
