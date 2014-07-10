/*****************************************************************************
******************************************************************************
**                                                                          **
**  Copyright (c) 2002 Videon Central, Inc.                                 **
**  All rights reserved.                                                    **
**                                                                          **
**  The computer program contained herein contains proprietary information  **
**  which is the property of Videon Central, Inc.  The program may be used  **
**  and/or copied only with the written permission of Videon Central, Inc.  **
**  or in accordance with the terms and conditions stipulated in the        **
**  agreement/contract under which the programs have been supplied.         **
**                                                                          **
******************************************************************************
*****************************************************************************/

/**
 * @file
 * vdvd_types.h
 *
 * $Id: vdvd_types.h,v 1.41 2007/01/26 20:30:43 rbehe Exp $
 */

#ifndef VDVD_TYPES_H
#define VDVD_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* these need defined before including arch.h */
#define VDVD_BIG_ENDIAN     0
#define VDVD_LITTLE_ENDIAN  1

#include "arch.h"
#include "vcitypes.h"

typedef uint32 TIME90k;
typedef uint32 TIME45k;

typedef unsigned char UBYTE;
typedef int           INT;

/*
 * User configuration settings
 */
typedef struct tagVDVD_USER_CONFIG
{
    USHORT  size;
    UCHAR   dvd_menu_language;
    UCHAR   audio_language;
    UCHAR   subtitle_language;
    UCHAR   tv_type;
    UCHAR   video_format;
    UCHAR   spdif;
    UCHAR   country;
    UCHAR   parental;
    UCHAR   parental_on_off;
    UCHAR   parental_password[4];
    USHORT  crc;
} VDVD_USER_CONFIG;

typedef enum tag_VDVD_PARENTAL_STATE
{
    PARENTAL_ON  = 1,
    PARENTAL_OFF = 2
} VDVD_PARENTAL_STATE;

/*
 * Media formats
 */
typedef enum tag_VDVD_MEDIA_FORMAT
{
    MEDIA_FORMAT_NODISC         = 0,
    MEDIA_FORMAT_DVD_A          = 0x00000001,
    MEDIA_FORMAT_DVD_V          = 0x00000002,
    MEDIA_FORMAT_HDDVD_STANDARD = 0x00000004,
    MEDIA_FORMAT_HDDVD_ADVANCED = 0x00000008,
    MEDIA_FORMAT_BDROM          = 0x00000010,
    MEDIA_FORMAT_VCD            = 0x00000020,
    MEDIA_FORMAT_SVCD           = 0x00000040,
    MEDIA_FORMAT_CDDA           = 0x00000080,
    MEDIA_FORMAT_MP3            = 0x00000100,
    MEDIA_FORMAT_URD            = 0x00000200,
    MEDIA_FORMAT_VR             = 0x00000400,
    MEDIA_FORMAT_READ_ERROR     = 0x00000800,
    MEDIA_FORMAT_CANNOT_PLAY    = 0x00010000,
    MEDIA_FORMAT_UNKNOWN        = 0xffffffff
} VDVD_MEDIA_FORMAT;

/*
 * Title Information
 */
typedef enum tag_VDVD_TITLE_INFO
{
    VDVD_TITLE_UNKNOWN          = 0,
    VDVD_TITLE_HDMV             = 0x00000001,
    VDVD_TITLE_BDJ              = 0x00000002,
    VDVD_TITLE_BD_MOVIE         = 0x00000004,
    VDVD_TITLE_BD_INTERACTIVE   = 0x00000008,
    VDVD_TITLE_BD_SLIDESHOW     = 0x00000010,
    VDVD_TITLE_ACCESS_PROHIBIT  = 0x00000020,
    VDVD_TITLE_OSD_PROHIBIT     = 0x00000040
} VDVD_TITLE_INFO;

/*
 * Aspect Ratios
 */
typedef enum
{
    VDVD_ASPECT_RATIO_4X3_PANSCAN = 0,
    VDVD_ASPECT_RATIO_4X3_LETTERBOX,
    VDVD_ASPECT_RATIO_4X3_LETTERBOX_REMOVED,
    VDVD_ASPECT_RATIO_16X9,
    VDVD_ASPECT_RATIO_16X9_WINDOWBOX
} VDVD_ASPECT_RATIO;

/*
 * Stop modes
 */
typedef enum
{
    VDVD_STOP_MODE_DEFAULT      = 0,
    VDVD_STOP_MODE_FORCED       = 0x0001,
    VDVD_STOP_MODE_DECODE_STOP  = 0x0002
} VDVD_STOP_MODE;

/*
 * Video Formats
 * NOTE: This covers all valid formats for BD content
 */
typedef enum
{
    VDVD_VIDEO_FORMAT_NTSC = 0,
    VDVD_VIDEO_FORMAT_480I_29_97 = VDVD_VIDEO_FORMAT_NTSC,
    VDVD_VIDEO_FORMAT_PAL,
    VDVD_VIDEO_FORMAT_576I_25 = VDVD_VIDEO_FORMAT_PAL,
    VDVD_VIDEO_FORMAT_480P,
    VDVD_VIDEO_FORMAT_576P,
    VDVD_VIDEO_FORMAT_1080I,
    VDVD_VIDEO_FORMAT_1080I_29_97 = VDVD_VIDEO_FORMAT_1080I,
    VDVD_VIDEO_FORMAT_1080I_25,
    VDVD_VIDEO_FORMAT_720P,
    VDVD_VIDEO_FORMAT_720P_59_94 = VDVD_VIDEO_FORMAT_720P,
    VDVD_VIDEO_FORMAT_720P_50,
    VDVD_VIDEO_FORMAT_720P_24,
    VDVD_VIDEO_FORMAT_720P_23_976,
    VDVD_VIDEO_FORMAT_1080P,
    VDVD_VIDEO_FORMAT_1080P_24 = VDVD_VIDEO_FORMAT_1080P,
    VDVD_VIDEO_FORMAT_1080P_23_976
} VDVD_VIDEO_FORMAT;

typedef enum 
{
    DFB_EXT_FRAME_RATE_UNKNOWN = 0,
    DFB_EXT_FRAME_RATE_23_976,
    DFB_EXT_FRAME_RATE_24,
    DFB_EXT_FRAME_RATE_25,
    DFB_EXT_FRAME_RATE_29_97,
    DFB_EXT_FRAME_RATE_50,
    DFB_EXT_FRAME_RATE_59_94
} FrameRate;

/* Repeat modes */
typedef enum
{
    VDVD_INFO_REPEAT_OFF = 0,
    VDVD_INFO_REPEAT_ALL,
    VDVD_INFO_REPEAT_TITLE,
    VDVD_INFO_REPEAT_CHAPTER,
    VDVD_INFO_REPEAT_A,
    VDVD_INFO_REPEAT_AB
} VDVD_INFO_REPEAT;


/*
 * SPDIF settings
 */
typedef enum
{
    VDVD_SPDIF_PCM = 0,
    VDVD_SPDIF_COMPRESSED
} VDVD_SPDIF;

/*
 * Command types
 */
typedef enum
{
    VDVD_COMMAND_STANDBY = 0,
    VDVD_COMMAND_PAUSE_ON,
    VDVD_COMMAND_PAUSE_OFF,
    VDVD_COMMAND_PLAY,
    VDVD_COMMAND_PLAY_LOCATION,
    VDVD_COMMAND_STOP,
    VDVD_COMMAND_EJECT,
    VDVD_COMMAND_NEXT_CHAPTER,
    VDVD_COMMAND_PREV_CHAPTER,
    VDVD_COMMAND_RESTART_CHAPTER,
    VDVD_COMMAND_FWD_PLAY,
    VDVD_COMMAND_BWD_PLAY,
    VDVD_COMMAND_STEP_FWD,
    VDVD_COMMAND_STEP_BWD,
    VDVD_COMMAND_REPEAT,
    VDVD_COMMAND_ENTER,
    VDVD_COMMAND_UP,
    VDVD_COMMAND_DOWN,
    VDVD_COMMAND_LEFT,
    VDVD_COMMAND_RIGHT,
    VDVD_COMMAND_NUMBER,
    VDVD_COMMAND_AUDIO_SELECT,
    VDVD_COMMAND_MENU,
    VDVD_COMMAND_TITLE,
    VDVD_COMMAND_ANGLE,
    VDVD_COMMAND_SETUP,
    VDVD_COMMAND_INFO,
    VDVD_COMMAND_GOTO_CHAPTER,
    VDVD_COMMAND_GOTO_TITLE,
    VDVD_COMMAND_GOTO_TIME,
    VDVD_COMMAND_SUBTITLE_ON_OFF,
    VDVD_COMMAND_SUBTITLE_SELECT,
    VDVD_COMMAND_SUBTITLE_USER_STYLE,
    VDVD_COMMAND_PARENTAL_PLAY,
    VDVD_COMMAND_ZOOM,
    VDVD_COMMAND_RETURN,
    VDVD_COMMAND_VOL_UP,
    VDVD_COMMAND_VOL_DOWN,
    VDVD_COMMAND_VOL_MUTE,
    VDVD_COMMAND_STILL_OFF,
    VDVD_COMMAND_POPUP_ON_OFF,
    VDVD_COMMAND_NUMERIC_BUTTON_SELECT,
    VDVD_COMMAND_RANDOM,
    VDVD_COMMAND_COLOR_KEY,
    VDVD_COMMAND_UP_RELEASE,
    VDVD_COMMAND_DOWN_RELEASE,
    VDVD_COMMAND_LEFT_RELEASE,
    VDVD_COMMAND_RIGHT_RELEASE,

    /* All command types above this line */
    VDVD_COMMAND_MAX
} VDVD_COMMAND;

/*
 * Status event types
 */
typedef enum
{
    VDVD_STATUS_PROHIBITED = 0,
    VDVD_STATUS_PAUSE,
    VDVD_STATUS_PLAY,
    VDVD_STATUS_STOP,
    VDVD_STATUS_OPEN,
    VDVD_STATUS_CLOSE,
    VDVD_STATUS_SLOW_F,
    VDVD_STATUS_SLOW_B,
    VDVD_STATUS_FAST_F,
    VDVD_STATUS_FAST_B,
    VDVD_STATUS_CHAPTER_SKIP,
    VDVD_STATUS_SKIP_NEXT,
    VDVD_STATUS_SKIP_PREV,
    VDVD_STATUS_STEP,
    VDVD_STATUS_REPEAT,
    VDVD_STATUS_A_B,
    VDVD_STATUS_NO_DISC,
    VDVD_STATUS_DISC_TYPE,
    VDVD_STATUS_LOADING,
    VDVD_STATUS_LOAD_COMPLETE,
    VDVD_STATUS_AUDIO_TYPE,
    VDVD_STATUS_RESUME_DISP,
    VDVD_STATUS_SUBTITLE_ON,
    VDVD_STATUS_SUBTITLE_OFF,
    VDVD_STATUS_SUBTITLE_CHG,
    VDVD_STATUS_ANGLE_CHG,
    VDVD_STATUS_AUDIO_CHG,
    VDVD_STATUS_INVALID_REGION,
    VDVD_STATUS_STANDBY_ON,
    VDVD_STATUS_STANDBY_OFF,
    VDVD_STATUS_ZOOM_ON,
    VDVD_STATUS_ZOOM_OFF,
    VDVD_STATUS_SETTMPPML,
    VDVD_STATUS_PARENTAL_PROHIBIT,
    VDVD_STATUS_VIDEO_FORMAT_UPDATE,
    VDVD_STATUS_BOOKMARK,
    VDVD_STATUS_GOTO,
    VDVD_STATUS_SETUP,
    VDVD_STATUS_ECHO,
    VDVD_STATUS_KEY_SHIFT,
    VDVD_STATUS_PROGRAM,
    VDVD_STATUS_ANGLE_ON,
    VDVD_STATUS_ANGLE_OFF,
    VDVD_STATUS_SURROUND,
    VDVD_STATUS_PROGRAM_CURSOR,
    VDVD_STATUS_RETURN,
    VDVD_STATUS_KARAOKE,
    VDVD_STATUS_VOCALFADE,
    VDVD_STATUS_POWER_ON,
    VDVD_STATUS_POWER_OFF,
    VDVD_STATUS_CLEAR,
    VDVD_STATUS_SHUFFLE,
    VDVD_STATUS_DISC_ERROR,
    VDVD_STATUS_AUDIO_NOT_SUPPORTED,
    VDVD_STATUS_FULLSTOP,
    VDVD_STATUS_STILL,
    VDVD_STATUS_DISPLAY_ASPECT_RATIO_UPDATE,
    VDVD_STATUS_NOT_IMPLEMENTED,
    VDVD_STATUS_TITLE_INFO,
    VDVD_STATUS_UNKNOWN,

    /* All status events above this line */
    VDVD_STATUS_MAX
} VDVD_STATUS_EVENT;

/* This enum is used with VDVD_STATUS_LOADING to indicate
 * what the player is currently loading */
typedef enum
{
    VDVD_LOADING_DISC = 0,
    VDVD_LOADING_TITLE,
} VDVD_INFO_LOADING;

/* Disc types */
typedef enum
{
    VDVD_INFO_DISCTYPE_UNKNOWN = 0,
    VDVD_INFO_DISCTYPE_DVD,
    VDVD_INFO_DISCTYPE_VCD,
    VDVD_INFO_DISCTYPE_CDDA,
    VDVD_INFO_DISCTYPE_MP3,
    VDVD_INFO_DISCTYPE_BDROM
} VDVD_INFO_DISCTYPE;

/* Audio Types */
typedef enum
{
    VDVD_INFO_AUDIOTYPE_DOLBY = 0,
    VDVD_INFO_AUDIOTYPE_MPEG,
    VDVD_INFO_AUDIOTYPE_LPCM,
    VDVD_INFO_AUDIOTYPE_DTS,
    VDVD_INFO_AUDIOTYPE_DTS_HD,
    VDVD_INFO_AUDIOTYPE_DD_PLUS,
    VDVD_INFO_AUDIOTYPE_MLP,
    VDVD_INFO_AUDIOTYPE_INVALID
}  VDVD_INFO_AUDIOTYPE;


/* Prohibited types */
typedef enum
{
    VDVD_INFO_PROH_NORMAL = 0,
    VDVD_INFO_PROH_NO_RESUME,
    VDVD_INFO_PROH_A_B,
    VDVD_INFO_PROH_CMD_NOT_SUPPORTED
} VDVD_INFO_PROH;

/* Invalid region types */
typedef enum
{
    VDVD_INFO_REGION_INVALID = 0,
    VDVD_INFO_REGION_NONE_SET
} VDVD_INFO_REGION;

typedef enum tagVDVD_REPEAT_MODES
{
    REPEAT_OFF = 0,
    REPEAT_ALL,
    REPEAT_TT,
    REPEAT_PTT,
    REPEAT_A_B,
    REPEAT_PROGRAM
} VDVD_REPEAT_MODES;

typedef enum _tagVDVD_CONNECT_COND
{
    VDVD_CONNECTION_ABORT             = 0,
    VDVD_CONNECTION_1                 = 1,
    VDVD_CONNECTION_5                 = 5,
    VDVD_CONNECTION_6                 = 6,
    VDVD_CONNECTION_SEAMLESS_ANGLE    = 7,
    VDVD_CONNECTION_NONSEAMLESS_ANGLE = 8,

    VDVD_CONNECTION_UNDEF
} VDVD_CONNECT_COND;

#define BUFSIZE        30               /* Size limited to data dump buffer header which is 31 sectors */
#define LBSIZE         (BUFSIZE*512)    /* local buffer n/512 = number of sectors */
#define LBUFSIZE       (LBSIZE+2048)

#define SUCCESS        0
#define FAILURE        1

#define DVD_SECTOR_SIZE                      2048
#define CDDA_SECTOR_SIZE                     2352
#define CD_MODE1_USER_DATA                   2048
#define CD_MODE2_USER_DATA                   2336
#define CD_MODE2_FORM1_USER_DATA             2048
#define CD_MODE2_FORM2_USER_DATA             2324  /* 2328 actually - but 4 "spare" bytes at end */
#define CD_ALLHDR_USER_ECC_SIZE              2340

/*
 * Blu-Ray Player Specific Defines
 */

#define BDROM_SPEC_VERSION                                89

/* player profiles */
#define BDROM_PROFILE_1_WITHOUT_OPTIONAL_FEATURES 0x00000100
#define BDROM_PROFILE_1_WITH_OPTIONAL_FEATURES    0x00010100
#define BDROM_PROFILE_2                           0x00030100
#define BDROM_PROFILE_3                           0x00080100

#ifdef __cplusplus
}
#endif

#endif  /* VDVD_TYPES_H */
