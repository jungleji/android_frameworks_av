#ifndef _RKBLURAY_USER_OPERATION_H_
#define _RKBLURAY_USER_OPERATION_H_

/* User Operation Event controller*/

#include "navigation.h"
#include "BlurayRegisters.h"
#include "BlurayPlayController.h"
#include <pthread.h> 


namespace RKBluray
{

class UOController
{
public:
    UOController(BlurayRegisters* reg,BlurayPlayController* controller);
    int UOCtrlSetRepeat(int mode);
    int UOCtrlChapterSearch(int chapter);
    int UOCtrlSelectButton(int buttonNumber);
    int UOCtrlMoveSelectButton(int direction);
    int UOCtrlActivateButton();
    int UOCtrlAudioChange(int index);
    int UOCtrlSubtitleChange(int index);
    int UOCtrlSubtitleSurfaceEnable(int enable);
    int UOCtrlTextSubtitleStyleChange(int style);
    int UOCtrlAngleChange(int angle);
    int UOCtrlPopupOn();
    int UOCtrlPopupOff();
    int UOCtrlTimeSearch(int64_t time);
private:
    BlurayRegisters * mRegister;
    BlurayPlayController* mPlayController;

    pthread_mutex_t mMutex;
};
}


#endif
