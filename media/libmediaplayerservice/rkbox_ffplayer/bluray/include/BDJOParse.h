#ifndef _RK_BLURAY_BDJOPARSE_
#define _RK_BLURAY_BDJOPARSE_

#include <jni.h>
#include "BlurayRegisters.h"

namespace RKBluray
{

class BDJOParse
{
public:
    BDJOParse(JavaVM *vm,char* path,BlurayRegisters* blurayregister);
    ~BDJOParse();


    int loadBDJO(char* fileName);
    
private:
    int getVersion(char* str);
    void getString(unsigned char*data, char* out, unsigned int length);
    void makeString(unsigned char* data, char** out, unsigned int length);
    jstring readjString(JNIEnv* env, unsigned char* data, unsigned int length);
    
    jobject parseBDJO(JNIEnv* env,unsigned char* data);
    jobject parseTerminalInfo(JNIEnv* env, unsigned char*& data);
    jobject parseAccessiblePlaylists(JNIEnv* env, unsigned char*& data);
    jobjectArray parseAppCacheInfo(JNIEnv* env, unsigned char*& data);
    jobjectArray parseAppManagementTable(JNIEnv* env, unsigned char*& data);
    
    JavaVM *mVm;
    
    jclass mTerminalInfor;
    jclass mAppCache;
    jclass mPlayListTable;
    jclass mAppEntry;
    jclass mProfiles;
    jclass mAppName;
    jclass mBdjo;
    
    char* mPath;
    BlurayRegisters* mRegister;
};


}
#endif
