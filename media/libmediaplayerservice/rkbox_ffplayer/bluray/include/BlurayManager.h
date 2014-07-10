#ifndef _RK_BLURAY_MANAGER_H_
#define _RK_BLURAY_MANAGER_H_


#include "Bluray.h"
#include "BlurayCallBack.h"


namespace RKBluray {

typedef enum tagMSG_MANAGER_TYPE
{
    MSG_MANAGER_EXIT = 0,
    MSG_MANAGER_MOUNT,
    MSG_MANAGER_UNMOUNT,
    MSG_MANAGER_INVALID
} MSG_MANAGER_TYPE;

typedef struct tagBLURAY_MANAGER_MESSAGE
{
    MSG_MANAGER_TYPE            mMsgType;
    int                         mPath;
} BlURAY_MANAGER_MESSAGE;

class BlurayManager
{
public:
    BlurayManager();
    ~BlurayManager();
    void mount(const char* path,const char* extension);
    bool createBlurayPlayer(const char* path);
    

    /*
    *  函数功能: 用于判断是否是蓝光目录
    *  Param: path: 播放路径
    *  说明: 该函数用于用于判断是否是蓝光文件(iso文件需要mount和losetup操作，映射成目录文件)。
    *        因为DVD后缀也是iso，映射成文件目录后，不具备蓝光的相关目录。本蓝光库不支持对DVD
    *        文件的解析和播放。
    */
    bool isBDDirectory(const char* path);

    /*
    *  函数功能: 本函数将启动蓝光相关文件的解析，并启动播放
    */
    void blurayPlay();
    Bluray* getBluray();
    char* getBasePath();

    /*
    *   函数功能: 该函数用于蓝光库的回调，蓝光所有的操作以及播放列表的设置等都通过该回调函数通知到RKBOXPlayer中
    */
    void  setPlayCallBack(BlurayPlayCallBack* callback);
private:
    /*
    *   函数功能: iso文件的mount和unmount函数。
    *   说明:     因为iso的mount和unmount需要线程具备root权限，而普通的apk和mediaServer不具备这个权限。
    *             因此，这里的的isoMount和isoUmount操作都只是通过往/data目录写一个sh文件，通过请求init.rc
    *             的server运行这个sh文件来实现iso的mount和unmount操作
    */
    int isoMount(const char* path);
    int isoUmount();

    /*
    *   函数功能: 蓝光iso mount和unmount的线程
    */
    static ULONG BlurayManagerTaskThread(void* params);

    // 用于处理蓝光所有的消息 
    Bluray* mBluray;

    /*  
    *  用于表示是否是iso文件，如果是iso文件，则需要将iso mount蓝光目录文件后才能做解析，退出时需要将iso unmount
    *  如果不是iso文件而直接是蓝光目录文件，则直接进行解析，退出时也不需要做unmount操作
    */
    bool                mISO;

    BlurayPlayCallBack*  mPlayBack;

    //  蓝光iso的mount和unmount操作的线程。因为iso 的mount和unmount操作在系统比较繁忙时会耗费大量时间，因此
    //  将蓝光的mount和unmount操作放置在线程中。
    int                 mTaskId;

    //  蓝光mount和unmount 线程的消息队列
    OS_MSG_Q_ID         mMsgQID;
};
}


#endif

