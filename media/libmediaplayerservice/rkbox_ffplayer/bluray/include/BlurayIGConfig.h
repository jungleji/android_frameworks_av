#ifndef _RK_BLURAY_IG_CONFIG_H_
#define _RK_BLURAY_IG_CONFIG_H_


#include "BlurayConfig.h"


namespace RKBluray {

#if 0
struct BlurayIGConfig
{
    int mPid;
    int mPlayItem;
    char mLanguangCode[4];
    char* mPath;
    unsigned long long mStartOffset;
    unsigned long long mEndOffset;

    BlurayIGConfig()
    {
        mPid = 0;
        mPlayItem = 0;
        mPath = NULL;
        mStartOffset = 0;
        mEndOffset = 0;
        memset(mLanguangCode,0,4);
    }

    ~BlurayIGConfig()
    {
        if(mPath != NULL)
        {
            free(mPath);
            mPath = NULL;
        }
    }
};
#else

class BlurayIGConfig:public BlurayConfig
{
public:
    char mLanguangCode[4];
    
    BlurayIGConfig()
    {
        memset(mLanguangCode,0,4);
    }
};


#endif

}
#endif
