#ifndef _MPLS_PARSE_H_
#define _MPLS_PARSE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdint.h>
#include "vcitypes.h"

#define BD_MARK_ENTRY   0x01
#define BD_MARK_LINK    0x02

#define MPLS_Sequential_PLAYLIST        1
#define MPLS_RANDOM_PLAYLIST            2
#define MPLS_SHUFFLE_PLAYLIST           3

/* SubPath_type values */
#define MPLS_SUBPATH_BROWSABLE_SLIDESHOW_AUDIO      2
#define MPLS_SUBPATH_IG_MENU                        3
#define MPLS_SUBPATH_TEXT_SUBTITLE                  4
#define MPLS_SUBPATH_OUTOFMUX_SYNC                  5
#define MPLS_SUBPATH_OUTOFMUX_ASYNC                 6
#define MPLS_SUBPATH_INMUX_SYNC                     7
#define MPLS_SUBPATH_PG                             8

/* Character code values */
#define MPLS_CHAR_CODE_UTF8             0x01
#define MPLS_CHAR_CODE_UTF16            0x02
#define MPLS_CHAR_CODE_SHIFT_JIS        0x03
#define MPLS_CHAR_CODE_EUC_KR           0x04
#define MPLS_CHAR_CODE_GB18030_2000     0x05
#define MPLS_CHAR_CODE_CN_GB            0x06
#define MPLS_CHAR_CODE_BIG5             0x07

/* video types */
#define MPLS_STREAM_CODING_TYPE_MPEG2               0x02
#define MPLS_STREAM_CODING_TYPE_AVC                 0x1B
#define MPLS_STREAM_CODING_TYPE_VC1                 0xEA

#define UOTABLE_MENU 0
#define UOTABLE_TITLE_SEARCH 1
#define UOTABLE_CHAPTER_SEARCH 2
#define UOTABLE_TIME_SEARCH 3
#define UOTABLE_SKIP_TO_NEXT_POINT 4
#define UOTABLE_SKIP_TO_PREV_POINT 5
#define UOTABLE_PLAY_FIRSTPLAY 6
#define UOTABLE_STOP 7
#define UOTABLE_PAUSE_ON 8
#define UOTABLE_PAUSE_OFF 9
#define UOTABLE_STILL 10
#define UOTABLE_FORWARD 11
#define UOTABLE_BACKWARD 12
#define UOTABLE_RESUME 13
#define UOTABLE_MOVEUP 14
#define UOTABLE_MOVEDOWN 15
#define UOTABLE_MOVELEFT 16
#define UOTABLE_MOVERIGHT 17
#define UOTABLE_SELECT 18
#define UOTABLE_ACTIVATE 19
#define UOTABLE_SELECT_AND_ACTIVATE 20
#define UOTABLE_PRIMARY_AUDIO_CHANGE 21
#define UOTABLE_ANGLE_CHANGE 22
#define UOTABLE_POP_ON 23
#define UOTABLE_POP_OFF 24
#define UOTABLE_PG_ENABLE_DISABLE 25
#define UOTABLE_PG_CHANGE 26
#define UOTABLE_SECONDARY_VIDEO_ENABLE_DISABLE 27
#define UOTABLE_SECONDARY_VIDEO_CHANGE 28
#define UOTABLE_SECONDARY_AUDIO_ENABLE_DISABLE 29
#define UOTABLE_SECONDARY_AUDIO_CHANGE 30
#define UOTABLE_PIG_PG_CHANGE 31

typedef struct
{
    uint8_t         menu_call : 1;
    uint8_t         title_search : 1;
    uint8_t         chapter_search : 1;
    uint8_t         time_search : 1;
    uint8_t         skip_to_next_point : 1;
    uint8_t         skip_to_prev_point : 1;
    uint8_t         play_firstplay : 1;
    uint8_t         stop : 1;
    uint8_t         pause_on : 1;
    uint8_t         pause_off : 1;
    uint8_t         still : 1;
    uint8_t         forward : 1;
    uint8_t         backward : 1;
    uint8_t         resume : 1;
    uint8_t         move_up : 1;
    uint8_t         move_down : 1;
    uint8_t         move_left : 1;
    uint8_t         move_right : 1;
    uint8_t         select : 1;
    uint8_t         activate : 1;
    uint8_t         select_and_activate : 1;
    uint8_t         primary_audio_change : 1;
    uint8_t         angle_change : 1;
    uint8_t         popup_on : 1;
    uint8_t         popup_off : 1;
    uint8_t         pg_enable_disable : 1;
    uint8_t         pg_change : 1;
    uint8_t         secondary_video_enable_disable : 1;
    uint8_t         secondary_video_change : 1;
    uint8_t         secondary_audio_enable_disable : 1;
    uint8_t         secondary_audio_change : 1;
    uint8_t         pip_pg_change : 1;
} MPLS_UO;

typedef struct
{
    uint8_t         stream_type;     // 1: main path;和音视频文件在同一个文件中  2:  in subpath,单独一个m2ts文件，需要单独打开解析
    uint8_t         coding_type;
    uint16_t        pid;
    uint8_t         subpath_id;
    uint8_t         subclip_id;
    uint8_t         format;
    uint8_t         rate;
    uint8_t         char_code;
    uint8_t         lang[4];        // 语言描叙，比如eng
    // Secondary audio specific fields
    uint8_t         sa_num_primary_audio_ref;
    uint8_t        *sa_primary_audio_ref;
    // Secondary video specific fields
    uint8_t         sv_num_secondary_audio_ref;
    uint8_t         sv_num_pip_pg_ref;
    uint8_t        *sv_secondary_audio_ref;
    uint8_t        *sv_pip_pg_ref;
} MPLS_STREAM;

typedef struct
{
    uint8_t         num_video;
    uint8_t         num_audio;
    uint8_t         num_pg;
    uint8_t         num_ig;
    uint8_t         num_secondary_audio;
    uint8_t         num_secondary_video;
    uint8_t         num_pip_pg;
    MPLS_STREAM    *video;
    MPLS_STREAM    *audio;
    MPLS_STREAM    *pg;
    MPLS_STREAM    *ig;
    MPLS_STREAM    *secondary_audio;
    MPLS_STREAM    *secondary_video;
} MPLS_STN;

typedef struct
{
    char            clip_id[6];   // 对应clip文件名和ts文件名
    char            codec_id[5];  //  文件后缀.m2ts
    uint8_t         stc_id;
} MPLS_CLIP;

typedef struct
{
    uint8_t         is_multi_angle;
    uint8_t         connection_condition;                /* Connection with previous play item (bd_mpls_play_item_connection_t) */
    uint32_t        in_time;                             /* Start time in 45kHz */
    uint32_t        out_time;                            /* Stop time in 45kHz */
    MPLS_UO         uo_mask;
    uint8_t         random_access_flag;
    uint8_t         still_mode;                          /* Still mode (bd_mpls_play_item_still_t) */
    uint16_t        still_time;                          /* Still time for BD_MPLS_PLAY_ITEM_STILL_FINITE (second?) */
    uint8_t         angle_count;
    uint8_t         is_different_audio;
    uint8_t         is_seamless_angle;
    MPLS_CLIP       *clip;
    MPLS_STN        stn;
    MPLS_STN        ext_stn;
} MPLS_PI;

typedef struct
{
    uint8_t         mark_id;
    uint8_t         mark_type;
    uint16_t        play_item_ref;
    uint32_t        time;
    uint16_t        entry_es_pid;
    uint32_t        duration;
} MPLS_PLM;

typedef struct
{
    uint8_t         playback_type;
    uint16_t        playback_count;
    MPLS_UO         uo_mask;
    uint8_t         random_access_flag;
    uint8_t         audio_mix_flag;
    uint8_t         lossless_bypass_flag;
} MPLS_AI;

typedef struct
{
    uint8_t         connection_condition;
    uint8_t         is_multi_clip;
    uint32_t        in_time;
    uint32_t        out_time;
    uint16_t        sync_play_item_id;
    uint32_t        sync_pts;
    uint8_t         clip_count;
    MPLS_CLIP       *clip;
} MPLS_SUB_PI;

typedef struct
{
    uint8_t         type;
    uint8_t         is_repeat;
    uint8_t         sub_playitem_count;
    MPLS_SUB_PI     *sub_play_item;
} MPLS_SUB;

typedef struct {
    uint32_t        time;
    uint16_t        xpos;
    uint16_t        ypos;
    uint8_t         scale_factor;
} MPLS_PIP_DATA;


/*
* 2013.11.4 for bluray 3D
*/
typedef struct {
    uint16_t        clip_ref;
    uint8_t         secondary_video_ref;
    uint8_t         timeline_type;
    uint8_t         luma_key_flag;
    uint8_t         upper_limit_luma_key;
    uint8_t         trick_play_flag;

    uint16_t        data_count;
    MPLS_PIP_DATA   *data;
} MPLS_PIP_METADATA;


typedef struct
{
    uint32_t        type_indicator;
    uint32_t        type_indicator2;
    uint32_t        list_pos;
    uint32_t        mark_pos;
    uint32_t        ext_pos;
    MPLS_AI         app_info;
    uint16_t        list_count;
    uint16_t        sub_count;
    uint16_t        mark_count;
    MPLS_PI        *play_item;
    MPLS_SUB       *sub_path;
    MPLS_PLM       *play_mark;

    // 3D extension data (profile 5, version 2.4)
    uint16_t        ext_sub_count;
    MPLS_SUB       *ext_sub_path;  // sub path entries extension

    // extension data (Picture-In-Picture metadata)
    uint16_t           ext_pip_data_count;
    MPLS_PIP_METADATA *ext_pip_data;  // pip metadata extension
} MPLS_PL;

typedef enum 
{
    Unknown = 0,
    MPEG1_VIDEO = 0x01,
    MPEG2_VIDEO = 0x02,
    MVC_VIDEO = 0x20,
    AVC_VIDEO = 0x1b,
    VC1_VIDEO = 0xea,
    MPEG1_AUDIO = 0x03,
    MPEG2_AUDIO = 0x04,
    LPCM_AUDIO = 0x80,
    AC3_AUDIO = 0x81,
    AC3_PLUS_AUDIO = 0x84,
    AC3_PLUS_SECONDARY_AUDIO = 0xA1,
    AC3_TRUE_HD_AUDIO = 0x83,
    DTS_AUDIO = 0x82,
    DTS_HD_AUDIO = 0x85,
    DTS_HD_SECONDARY_AUDIO = 0xA2,
    DTS_HD_MASTER_AUDIO = 0x86,
    PRESENTATION_GRAPHICS = 0x90,
    INTERACTIVE_GRAPHICS = 0x91,
    TEXT_SUBTITLE = 0x92
}TSStreamType;

extern int mpls_mallocTimer;
MPLS_PL* mpls_parse(char *path);
void mpls_free(MPLS_PL *pl);
MPLS_AI* getAppInfo(MPLS_PL* pl);
MPLS_SUB* getSubPath(MPLS_PL* pl,int subPathNumber);
MPLS_PI*  getPlayItem(MPLS_PL* pl,int itemId);
MPLS_PLM* getPlayMark(MPLS_PL* pl);
MPLS_UO* getAppInforUOTable(MPLS_PL* pl);
MPLS_UO* getPlayItemUOTable(MPLS_PL* pl,int playItemId);
int     getUOEnable(MPLS_PL* pl,int playItem,int index);
MPLS_SUB* getSubPath(MPLS_PL* pl,int index);

#ifdef __cplusplus
}
#endif

#endif // _MPLS_PARSE_H_
