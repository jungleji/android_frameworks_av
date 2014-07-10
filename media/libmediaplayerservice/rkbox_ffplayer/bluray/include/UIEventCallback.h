#ifndef _UI_EVENT_CALLBACK_
#define _UI_EVENT_CALLBACK_

namespace RKBluray
{

class UIEventCallback
{
public:
    virtual void setVideoType(int type) = 0;
    virtual void setVideoWidthAndHeight(int width,int height) = 0;
    virtual void setVideoFrameRate(int rate) = 0;
    virtual ~UIEventCallback(){};
};
}

#endif
