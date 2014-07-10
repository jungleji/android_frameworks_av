/*
* this file define by hehua to parse index.bdmv when playing bluRay
*
*The ¡°Index Table¡± is a top-level table that defines the Titles and the Top Menu of a BD-ROM disc. This
*table contains entry points for all of the Titles and the Top Menu. The Player references this table
*whenever a Title or Menu is to be executed e.g. whenever the Title Search or Menu Call operation is called,
*the player refers to this table to determine the corresponding Movie Object/BD-J Object that is to be
*executed.

*The Index Table also has an entry to a Movie Object/BD-J Object designated for ¡°First Playback¡± ¨C this
*can be used by Content Providers to perform automatic playback. When the disc is loaded, the player
*refers to the ¡°First Playback¡± entry to determine the corresponding Movie Object or BD-J Object that shall
*be executed.
*/

#ifndef INDEXPARSE_H_
#define INDEXPARSE_H_
extern "C"
{
#include "file.h"
}

namespace RKBluray {

 /**
  * Index constants
  */
#define INDEX_MAX_TITLES    (999)

#define BD_2D  0
#define BD_3D  1

/**
 * Object Type
 */
typedef enum tagINDEX_OBJ_TYPE
{
    INDEX_OBJ_HDMV = 0,
    INDEX_OBJ_BDJ,
    INDEX_OBJ_INVALID
} INDEX_OBJ_TYPE;

/**
 * Title Playback Type
 */
typedef enum tagINDEX_TITLE_PB_TYPE
{
    INDEX_TITLE_PB_MOVIE = 0,
    INDEX_TITLE_PB_INTERACTIVE,
    INDEX_TITLE_PB_INVALID
} INDEX_TITLE_PB_TYPE;

/**
 * Title Access Type
 */
typedef enum tagINDEX_TITLE_ACCESS_TYPE
{
    INDEX_TITLE_ACCESS_PERMIT = 0,
    INDEX_TITLE_ACCESS_PROHIBIT,
    INDEX_TITLE_ACCESS_PROHIBIT_NO_OSD,
    INDEX_TITLE_ACCESS_INVALID
} INDEX_TITLE_ACCESS_TYPE;

/**
 * Object ID
 */
typedef union tagINDEX_OBJ_ID
{
    unsigned int   ulHdmvMobjID;
    unsigned char   ubBdjFileTag[6];
} INDEX_OBJ_ID;


typedef struct tagINDEX_OBJ_REF
{
    INDEX_OBJ_TYPE          tObjType;
    INDEX_OBJ_ID            tObjectId;
    INDEX_TITLE_PB_TYPE     tPlaybackType;
    INDEX_TITLE_ACCESS_TYPE tAccessType;

    tagINDEX_OBJ_REF()
    {
        tObjType = INDEX_OBJ_INVALID;
        tPlaybackType = INDEX_TITLE_PB_INVALID;
        tAccessType = INDEX_TITLE_ACCESS_INVALID;
        tObjectId.ulHdmvMobjID = -1;
    }
} INDEX_OBJ;

class AppInfoBDMV
{
public:
    int mMode;   // 0:2D, 1:3D
    int mSSContentExistFlag; 
    int mVideoFormat;  // 1: 480i, 2: 576i, 3: 480p, 4: 1080i, 5: 720p, 6: 1080p, 7: 576p.
    int mFrameRate;  // 1: 24000/1001 FPS,  2: 24 FPS,  3: 25 FPS, 4: 30000/1001 FPS, 6: 50 FPS,7: 60000/1001 FPS.
    char ContentProviderData[33]; // 32
};

class ExtensionData
{
public:
    int mType;
    int mVersion;

    unsigned int mStartAddress;
    int mLength;
};

class IndexTable
{
    private:
        static const char* TypeIndicator;   
        static const int   Version;

        unsigned int fileSize;
        
        int numberOfTitle;
        int mNumberOfExtension;
        
        const char* mPath;   // pointer to BD-ROM disc root directory
        INDEX_OBJ  mFirstPlay;
        INDEX_OBJ  mTopMenu;
        INDEX_OBJ  mTitle[INDEX_MAX_TITLES];
        ExtensionData** mExtension;
        AppInfoBDMV mAppInfoBDMV;
        void parseAppInfoBDMV(FILE_H* file);
        int parseExtensionData(FILE_H* file);
    public:
        IndexTable(const char* path);
        ~IndexTable();
        int LoaderIndexTable();
        int ParseIndex(FILE_H* file);
        int CheckIndicator(FILE_H* file);        // check Indicator and version
        int indexLoadIndex(FILE_H* file,unsigned int address);
        int parseFirstPlay(FILE_H* file);
        int parseTopMenu(FILE_H* file);
        int parseTitle(FILE_H* file,int index);

        INDEX_OBJ* getFirstPlay();
        INDEX_OBJ* getTopMenu();
        INDEX_OBJ* getTitle(int index);
        int getTitleNumber();
};
}

#endif
