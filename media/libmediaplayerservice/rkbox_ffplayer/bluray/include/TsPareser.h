#ifndef RK_BLURAY_TS_PARSER_H_
#define RK_BLURAY_TS_PARSER_H_

#include "BlurayConfig.h"
#include "stdio.h"
#include "Mpeg.h"
#include <utils/Vector.h>

extern "C"
{
#include "vcitypes.h"
#include "osapi.h"
}

using namespace android;

/*
*  define by hh@rock-chips.com for parse Bluray's m2ts file
*/

namespace RKBluray
{

#define BD_PACKET_SIZE 192
#define TS_PACKET_SIZE 188
#define MAX_PES_PACKAET (200*1024)//65535


class TsData
{
public:
    char* mData;
    int mSize;

    TsData()
    {
        mData = new char[MAX_PES_PACKAET];
        mSize = 0;
    }

    ~TsData()
    {
        if(mData != NULL)
        {
            delete[] mData;
            mData = NULL;
        }

        mSize = 0;
    }
};

class TsParser
{
public:
    TsParser();
    ~TsParser();
    void resetParser();
    int openFile(BlurayConfig* config);
    void closeFile();
    void setPid(int pid);
    int getTsPackets();
    int readPacket(char* buf);
    int parseTsPacket(char* data,int size);
    int parsePESPacket(char* buffer,int inSize);
private:
    /*
    *  ���ļ���·��
    */
    char* mPath;

    /*
    * ���������ļ�
    */
    FILE*     mFile;

    /*
    * ts����·��
    */
    int             mPid; 

    /*
    * �ļ�ƫ�ƵĿ�ʼ��ַ
    */
    unsigned long long mStartOffset;

    /*
    * �ļ�ƫ�ƵĽ�����ַ
    */
    unsigned long long mEndOffset;

    /*
    * ts������������continuityCounter������
    */
    int mLastCC;

    /*
    *  ��������
    */
    Vector<TsData*>* mData;

    /*
    * 
    */
    int mPosition;
    
    /*
    * ��ȡts����buffer
    */
    char* mReadBuffer;

    TsData* mPesPacket;
    
    
    unsigned char getByte(char* buf);
    unsigned short getWORD(char*buf);
    int get3Byte(char* buf);
    int get4Byte(char* buf);
    
};
    
}

#endif
