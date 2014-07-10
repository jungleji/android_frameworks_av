/*
* this file define by hehua to parse MovieObject.bdmv when playing bluRay.

*A ¡°Movie Object¡± consists of an executable navigation command program (HDMV program). This
*enables dynamic scenario description. Movie Objects exist in the layer above PlayLists.
*Navigation commands in a Movie Object can launch PlayList playback or another Movie Object. This
*enables the Content Provider to define a set of Movie Objects for managing playback of PlayLists in
*accordance with a user¡¯s interaction and preferences
*/


#ifndef MOVIEOBJECTPARSE_H_
#define MOVIEOBJECTPARSE_H_


extern "C"
{
#include "file.h"
}

#include "NavigationCommand.h"


namespace RKBluray {


/**
 * Movie Object constants
 */
#define MVOBJ_MAX_MOVIE_OBJECTS         1001
#define MVOBJ_MAX_NAV_COMMANDS          ( (600 * 1024) / sizeof(NavigationCommand) )
#define MVOBJ_MAX_BUTTON_NAV_COMMANDS   255

#define ENTRY_MARK 0x01
#define LINK_POINT 0x02

/**
 * Movie Object terminal information
 */
typedef struct tagMVOBJ_TERMINFO
{
    bool fResumeIntentionFlag;
    bool fMenuCallMask;
    bool fTitleSearchMask;
} MVOBJ_TERMINFO;


class MovieObject{
    public:
        MovieObject(const char*path);
        virtual ~MovieObject();
        int loadMovieObject();
        NavigationCommand* getMovieObjectNavigationCmd(int objectId);
        unsigned short getNumberOfNavigationCmd(int objectId);
        int getMovieObjectCount();
        MVOBJ_TERMINFO* getTermInfor(int id);

        static int malloTimer;
    private:
        static const char* TypeIndicator;   
        static const int   Version;
        const char* mPath;
        int mExtensinStartAddress;
        int numberOfMovieObject;
        MVOBJ_TERMINFO*      mTermInfo;
        unsigned short       usNumberOfNavCmds[MVOBJ_MAX_MOVIE_OBJECTS];
        NavigationCommand*   mNavCmd[MVOBJ_MAX_MOVIE_OBJECTS];
        
        int parseExtensionData(FILE_H* file);
        int parseMovieObject(FILE_H* file);
        int parseMovieObjectIndicator(FILE_H* file);
        int loadMovieObject(FILE_H* file);
        int loadMovieTermInfor(FILE_H* file,int index);
        int loadMovieNavigationCmd(FILE_H* file,int index);
};

}

#endif
