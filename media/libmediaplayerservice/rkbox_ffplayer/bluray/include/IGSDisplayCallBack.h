#ifndef _IGS_DISPLAY_CALLBACK_H_
#define _IGS_DISPLAY_CALLBACK_H_

namespace RKBluray{

class IGSDisplayCallBack
{
public:
    virtual void displayIGS(bool show) = 0;
    virtual ~IGSDisplayCallBack(){};
};

}



#endif
 
