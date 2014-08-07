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
    *  ��������: �����ж��Ƿ�������Ŀ¼
    *  Param: path: ����·��
    *  ˵��: �ú������������ж��Ƿ��������ļ�(iso�ļ���Ҫmount��losetup������ӳ���Ŀ¼�ļ�)��
    *        ��ΪDVD��׺Ҳ��iso��ӳ����ļ�Ŀ¼�󣬲��߱���������Ŀ¼��������ⲻ֧�ֶ�DVD
    *        �ļ��Ľ����Ͳ��š�
    */
    bool isBDDirectory(const char* path);

    /*
    *  ��������: ��������������������ļ��Ľ���������������
    */
    void blurayPlay();
    Bluray* getBluray();
    char* getBasePath();

    /*
    *   ��������: �ú������������Ļص����������еĲ����Լ������б�����õȶ�ͨ���ûص�����֪ͨ��RKBOXPlayer��
    */
    void  setPlayCallBack(BlurayPlayCallBack* callback);
private:
    /*
    *   ��������: iso�ļ���mount��unmount������
    *   ˵��:     ��Ϊiso��mount��unmount��Ҫ�߳̾߱�rootȨ�ޣ�����ͨ��apk��mediaServer���߱����Ȩ�ޡ�
    *             ��ˣ�����ĵ�isoMount��isoUmount������ֻ��ͨ����/dataĿ¼дһ��sh�ļ���ͨ������init.rc
    *             ��server�������sh�ļ���ʵ��iso��mount��unmount����
    */
    int isoMount(const char* path);
    int isoUmount();

    /*
    *   ��������: ����iso mount��unmount���߳�
    */
    static ULONG BlurayManagerTaskThread(void* params);

    // ���ڴ����������е���Ϣ 
    Bluray* mBluray;

    /*  
    *  ���ڱ�ʾ�Ƿ���iso�ļ��������iso�ļ�������Ҫ��iso mount����Ŀ¼�ļ���������������˳�ʱ��Ҫ��iso unmount
    *  �������iso�ļ���ֱ��������Ŀ¼�ļ�����ֱ�ӽ��н������˳�ʱҲ����Ҫ��unmount����
    */
    bool                mISO;

    BlurayPlayCallBack*  mPlayBack;

    //  ����iso��mount��unmount�������̡߳���Ϊiso ��mount��unmount������ϵͳ�ȽϷ�æʱ��ķѴ���ʱ�䣬���
    //  �������mount��unmount�����������߳��С�
    int                 mTaskId;

    //  ����mount��unmount �̵߳���Ϣ����
    OS_MSG_Q_ID         mMsgQID;
};
}


#endif

