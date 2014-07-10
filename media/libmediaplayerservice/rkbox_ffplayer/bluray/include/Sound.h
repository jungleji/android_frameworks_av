#ifndef _BLURAY_SOUND_
#define _BLURAY_SOUND_

//#include "../file/file.h"

/**
 * Clip Info Database Status types
 */
//typedef ERR_CODE  SOUNDDB_STATUS;

#include "stdio.h"
#include "LOGD.h"


namespace RKBluray{

#define SOUND_SUCCESS             0
#define SOUND_FAILURE            -1
#define SOUND_NULL_PTR           -2
#define SOUND_FILE_ERROR         -3
#define SOUND_INVALID_FORMAT     -4

#define MAX_NUMBER_SOUNDS          128

#define SOUND_ENDIAN 1


typedef struct SOUND_ATTRIBUTES
{
    unsigned char   channel_configuration;
    unsigned char   bits_per_sample;
    unsigned int   sampling_frequency;

    SOUND_ATTRIBUTES()
    {
        channel_configuration = 0;
        sampling_frequency = 0;
        bits_per_sample = 0;
    }
} SOUND_ATTRIBUTES;

typedef struct SOUND_LOCATION
{
    unsigned int  sound_data_start_address;
    unsigned int  sound_frame_number;

    SOUND_LOCATION()
    {
        sound_data_start_address = 0;
        sound_frame_number = 0;
    }
} ;

typedef struct tagSOUNDINDEX
{
    unsigned int        length;
    unsigned char       number_of_sound_entries;
    SOUND_ATTRIBUTES    *sound_attributes;
    SOUND_LOCATION      *sound_location;
} SOUNDINDEX;

typedef struct tagEXTENSIONDATA
{
} EXTENSIONDATA;


class SoundData
{
public:
    int mLength;
    unsigned char* mData;
public:
    SoundData()
    {
        mLength = 0;
        mData = NULL;
    }

    ~SoundData()
    {
        mLength = 0;
        if(mData != NULL)
        {
            delete[] mData;
            mData = NULL;
        }
    }
};

class Sound
{
private:
    unsigned char         type_indicator[4];
    unsigned char         version_number[4];
    unsigned int          SoundData_start_address;
    unsigned int          ExtensionData_start_address;
    SoundData             *mSoundData;
    SOUNDINDEX            mSoundIndex;
    EXTENSIONDATA         mExtensionData;

    const char*           mPath;

    int parseAttributes(FILE*   file, SOUND_ATTRIBUTES* attributes);
    int parseSoundData(FILE* file,int index);
    void soundDataSwith(unsigned char* src,unsigned char* dst,int length);
public:
    Sound(const char* path);
    ~Sound();
    int parseSound();
    SoundData* getSoundData(int index);
    SOUND_ATTRIBUTES *getSoundAttributes(int index);
};

}
#endif
