#ifndef _RK_PLAY_CONTROL_CALLBACK_H_
#define _RK_PLAY_CONTROL_CALLBACK_H_

#include "NavigationCommand.h"
extern "C"
{
#include "libavcodec/ig.h"
}

namespace RKBluray{

class PlayControlEventCallback
{
public:
    virtual int igsActivateButton(IGCommand* cmd,int number) = 0;
    virtual int igsPlayButtonSound(int soundId) = 0;
    virtual ~PlayControlEventCallback(){};
};
}
#endif
