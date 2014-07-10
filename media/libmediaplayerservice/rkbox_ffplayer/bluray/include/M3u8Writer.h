#ifndef BLURAY_M3U8WRITER_H_

#define BLURAY_M3U8WRITER_H_

#include "navigation.h"
#include <stdio.h>
#include <pthread.h> 


namespace RKBluray{


class M3u8Writer
{
public:
    M3u8Writer(char* path);
    ~M3u8Writer();
    bool isPlayFileExsit(NAV_TITLE* title);
    void playTitle(NAV_TITLE* title,int chapter);
    void playChapter(NAV_TITLE* title,int mode,int inPlayItem,unsigned int inTime,int outPlayItem,unsigned outTime);
    void playChapter(NAV_TITLE* title,int mode,int chapter);
    void playChapter_RepeatOff(NAV_TITLE* title,int chapterNum);
    void playChapter_RepeatChapter(NAV_TITLE* title,int chapterNum);
    void setPlayMode(int mode);
private:
    FILE* mFd;
    char* mPath;
    int   mPlayMode;

 //   const static char* HEAD;
    const static char* START;
    const static char* DURATION;
    const static char* DISCONTINUITY;
    const static char* EXTINF;
    const static char* SEEKSTART;
    const static char* SEEKEND;
    const static char* PLAYSEG;
    const static char* PLAYSEGPOSITION;
    const static char* PLAYSEGTIME;

 //   const static char* STREAM;

    const static char* LINE;
    const static char* END;

//    pthread_mutex_t mMutex;
    
    int getDuration(MPLS_PL*pl, int inPlayItem,int outPlayItem);
};
}

#endif
