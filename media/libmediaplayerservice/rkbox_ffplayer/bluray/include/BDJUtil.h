#ifndef _RKBLURAY_BDJ_UTIL_
#define _RKBLURAY_BDJ_UTIL_

#include <jni.h>

namespace RKBluray
{

class BDJUtil
{
public:
    static jobject bdjMakeObject(JNIEnv* env,jclass obj_class, const char* param, ...);
    static jobject bdjMakeObject(JNIEnv* env,const char* clz, const char* param, ...);
    static jobjectArray bdjMakeArray(JNIEnv* env,const char* clz, int count);
    static jobjectArray bdjMakeArray(JNIEnv* env,jclass obj_class, int count);
};


}

#endif
