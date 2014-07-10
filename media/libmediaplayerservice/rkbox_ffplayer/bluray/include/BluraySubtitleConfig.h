#ifndef _RK_BLURAY_SUBTITLE_CONFIG_H_
#define _RK_BLURAY_SUBTITLE_CONFIG_H_

#include "BlurayConfig.h"


namespace RKBluray {

// The Sutitle Code Type ±àÂë
#define Sutitle_Code_PG  0x90    // PG Í¼Æ¬×ÖÄ»
#define Sutitle_Code_TEXT  0x92  // Text ÎÄ×Ö×ÖÄ»

// The Subtitle Character Code
#define Sutitle_Character_UTF8  0x01
#define Sutitle_Character_UTF16BE  0x02
#define Sutitle_Character_Shirf_JIS  0x03
#define Sutitle_Character_KSC  0x04
#define Sutitle_Character_GB18030 0x05
#define Sutitle_Character_GB2312  0x06
#define Sutitle_Character_BIG5  0x07

class BluraySubtitleConfig:public BlurayConfig
{
public:
    int mCharacter;
    char mLanguage[4];

    BluraySubtitleConfig()
    {
        mCharacter = 0;
        memset(mLanguage,0,4);
    }
};

}
#endif
