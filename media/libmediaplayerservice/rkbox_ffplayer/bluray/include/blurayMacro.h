#ifndef _BLURAY_MACOR_H_
#define _BLURAY_MACOR_H_

#include "vcitypes.h"

namespace RKBluray {
/**
 * UO Masks
 */
const uint64 UOMASK_CHAPTER_SEARCH              = ((uint64)1 << 61);
const uint64 UOMASK_TIME_SEARCH                 = ((uint64)1 << 60);
const uint64 UOMASK_SKIP_TO_NEXT_POINT          = ((uint64)1 << 59);
const uint64 UOMASK_SKIP_TO_PREV_POINT          = ((uint64)1 << 58);
const uint64 UOMASK_STOP                        = ((uint64)1 << 56);
const uint64 UOMASK_PAUSE_ON                    = ((uint64)1 << 55);
const uint64 UOMASK_PAUSE_OFF                   = ((uint64)1 << 54);
const uint64 UOMASK_STILL_OFF                   = ((uint64)1 << 53);
const uint64 UOMASK_FORWARD_PLAY                = ((uint64)1 << 52);
const uint64 UOMASK_BACKWARD_PLAY               = ((uint64)1 << 51);
const uint64 UOMASK_RESUME                      = ((uint64)1 << 50);
const uint64 UOMASK_MOVE_UP_SELECTED_BUTTON     = ((uint64)1 << 49);
const uint64 UOMASK_MOVE_DOWN_SELECTED_BUTTON   = ((uint64)1 << 48);
const uint64 UOMASK_MOVE_LEFT_SELECTED_BUTTON   = ((uint64)1 << 47);
const uint64 UOMASK_MOVE_RIGHT_SELECTED_BUTTON  = ((uint64)1 << 46);
const uint64 UOMASK_SELECT_BUTTON               = ((uint64)1 << 45);
const uint64 UOMASK_ACTIVATE_BUTTON             = ((uint64)1 << 44);
const uint64 UOMASK_SELECT_AND_ACTIVATE_BUTTON  = ((uint64)1 << 43);
const uint64 UOMASK_PRIMARY_AUDIO_CHANGE        = ((uint64)1 << 42);
const uint64 UOMASK_ANGLE_CHANGE                = ((uint64)1 << 40);
const uint64 UOMASK_POPUP_ON                    = ((uint64)1 << 39);
const uint64 UOMASK_POPUP_OFF                   = ((uint64)1 << 38);
const uint64 UOMASK_PG_AND_ST_ENABLE            = ((uint64)1 << 37);
const uint64 UOMASK_PG_AND_ST_CHANGE            = ((uint64)1 << 36);
const uint64 UOMASK_SECONDARY_VIDEO_ENABLE      = ((uint64)1 << 35);
const uint64 UOMASK_SECONDARY_VIDEO_CHANGE      = ((uint64)1 << 34);
const uint64 UOMASK_SECONDARY_AUDIO_ENABLE      = ((uint64)1 << 33);
const uint64 UOMASK_SECONDARY_AUDIO_CHANGE      = ((uint64)1 << 32);
const uint64 UOMASK_PIP_PG_AND_ST_ENABLE        = ((uint64)1 << 31);
const uint64 UOMASK_PIP_PG_AND_ST_CHANGE        = ((uint64)1 << 30);


/* iStreamControl Video Coding Type */
typedef enum _tagPE_ISTREAMCTRL_VIDEO_TYPE
{
    VIDEO_TYPE_MPEG2 = 0,
    VIDEO_TYPE_AVC,
    VIDEO_TYPE_VC1,
    VIDEO_TYPE_MPEG1,

    /* this is always the last value */
    VIDEO_TYPE_INVALID
} PE_ISTREAMCTRL_VIDEO_TYPE, PE_ISTREAMCTRL_VIDEO_EXT_TYPE;


/*
* Bluray Play mode
*/
#define BD_2D  0
#define BD_3D  1

/*
* Bluray User Operation Return Code
*/
#define BLURAY_OPERATION_BDJ_UNSUPPORT 1
#define BLURAY_OPERATION_FORBIDDEN     2
#define BLURAY_OPERATION_UNSUPPORT     3
#define ALWAY_POPON_MENU               4


#define UO_PARAMETER_ERROR         100


#define USE_ION   0
}

#endif