#ifndef _RK_BLURAY_CONFIG_H_
#define _RK_BLURAY_CONFIG_H_

#include <stdlib.h>

namespace RKBluray {


class BlurayConfig
{
public:
    int mPid;
    int mPlayItem;
    int mCode;
    int mInMainPath;
    unsigned int mDuration;
    char*  mPath;
    unsigned long long mStartOffset;
    unsigned long long mEndOffset;

    BlurayConfig()
    {
        mPid = 0;
        mPlayItem = 0;
        mCode = 0;
        mDuration = 0;
        mInMainPath = 1;
        mPath = NULL;
        mStartOffset = 0;
        mEndOffset = 0;
    }

    ~BlurayConfig()
    {
        if(mPath != NULL)
        {
            free(mPath);
            mPath = NULL;
        }
    }
};


}
#endif
