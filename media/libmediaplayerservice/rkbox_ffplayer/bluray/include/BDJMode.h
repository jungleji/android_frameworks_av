#ifndef _RK_BLURAY_BDJMODE_
#define _RK_BLURAY_BDJMODE_

#include <jni.h>
#include "BlurayRegisters.h"
#include "BDJOParse.h"
#include "BlurayPlayController.h"

namespace RKBluray
{

class BDJMode
{
public:
    BDJMode(JavaVM *vm,char* path,BlurayRegisters* blurayregister);
    ~BDJMode();
    void setPlayController(BlurayPlayController* play);
    int playBDJ(char* path);
    
private:
    char* mPath;
    BlurayRegisters* mRegister;
    BDJOParse* mBDJOParser;
    BlurayPlayController* mPlayController;
};
}
#endif
