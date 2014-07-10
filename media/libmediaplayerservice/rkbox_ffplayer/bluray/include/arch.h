/******************************************************************************
*******************************************************************************
**                                                                           **
**  Copyright (c) 2006 Videon Central, Inc.                                  **
**  All rights reserved.                                                     **
**                                                                           **
**  The computer program contained herein contains proprietary information   **
**  which is the property of Videon Central, Inc.  The program may be used   **
**  and/or copied only with the written permission of Videon Central, Inc.   **
**  or in accordance with the terms and conditions stipulated in the         **
**  agreement/contract under which the programs have been supplied.          **
**                                                                           **
*******************************************************************************
******************************************************************************/
/**
 * @file arch.h
 *
 * $Revision: 1.12 $ 
 *
 * Used for configuring the build for different hardware and software options.
 * A new version of this file should be created for each project to meet the
 * specific hardware and software requirements.
 *
 */

#ifndef ARCH_H
#define ARCH_H

#define KSEG0(x) (x)
#define KSEG1(x) (x)

#define VDVD_PLATFORM_ENDIANESS VDVD_LITTLE_ENDIAN

/*
 * For each platform it is possible to define a number of sub-options:
 *
 *                  MACROVISION - Setting to FALSE stops the software trying to
 *                                enable copyprotection. Setting to TRUE causes
 *                                the software to enable copyprotection. .
 *
 *                 REGION_CHECK - Setting to TRUE enables region checking. Setting to
 *                                FALSE disables region checking.
 *
 *               CLOSED_CAPTION - Setting to TRUE enables closed caption data to be
 *                                transmitted to the video encoder. Setting to FALSE
 *                                disables closed caption.
 *
 */


/*
 * Hardware profiles.
 *
 * The following section hardware profiles which set-up the default values
 * MACROVISION, REGION_CHECK, AUTO_TV_SYSTEM, CLOSED_CAPTION for the
 * Unified or Plannet player.
 */
#define     CLOSED_CAPTION      FALSE
#define     MACROVISION         TRUE

#ifdef ENABLE_REGION_CHECKING
#  define REGION_CHECK   TRUE
#else
#  define REGION_CHECK   FALSE
#endif

/*
 * The following are the feature option which can be included
 * or selected to the system.
 */

#define DEFAULT_SYS_PARAM_0     0    /* Standard system parameter set */
#define DEFAULT_SYS_PARAM_1     1    /* Modified system parameter set */

#define KARAOKE_DEFAULT         0    /* Karaoke chip 1 */
#define KARAOKE_1               1    /* Karaoke chip 2 */
#define NO_KARAOKE              3    /* No Karaoke chip on board */

#define PROGRAM_MODE_DEFAULT    0    /* DVD program playback mode 1 */
#define PROGRAM_MODE_1          1    /* DVD program playback mode 2 */
#define PROGRAM_MODE_2          2    /* DVD program playback mode 3 */

#define REPEAT_MODE_DEFAULT     0    /* DVD repeat mode 1 */
#define REPEAT_MODE_1           1    /* DVD repeat mode 2 */

#define OSD_DEFAULT             0    /* On screen display type 1 */
#define OSD_1                   1    /* On screen display type 2 */
#define OSD_2                   2    /* On screen display type 3 */

#define KEY_PLAY_INTERPRET_DEFAULT  0  /* Perform standard PLAY key function        */
#define KEY_PLAY_INTERPRET_1        1  /* Perform PLAY/SELECT function for PLAY key */

#define MULTIPLE_SYSTEM_OFF     0    /* Disable multipe TV system                          */
#define MULTIPLE_SYSTEM_1       1    /* NTSC, PAL, PAL60 and auto TV switch can be selected */
#define MULTIPLE_SYSTEM_2       2    /* NTSC, PAL60 and auto TV switch can be selected      */
#define MULTIPLE_SYSTEM_3       3    /* NTSC, PAL and PAL60 can be selected                 */

#define RC_DEFAULT              0    /* Enable remote control default handling */
#define RC_1                    1    /* Enable remote control type 1 handling  */

#define TV_OUTPUT_DEFAULT_NTSC  0    /* Set NTSC TV system in power up */
#define TV_OUTPUT_DEFAULT_PAL   1    /* Set PAL TV system in power up  */

#define DIGITAL_AUDIO_DEFAULT   0    /* Standard Digital audio mode selection */
#define DIGITAL_AUDIO_1         1    /* Enhanced Digital audio mode selection */

#define AUDIO_MUTE_DEFAULT      0    /* Standard master audio mute */
#define AUDIO_MUTE_1            1    /* Master audio mute type 1   */
#define AUDIO_MUTE_2            2    /* Master audio mute type 2   */

#define SUBTITLE_DEFAULT        0    /* Standard on screen subtitle selection */
#define SUBTITLE_1              1    /* New on screen subtitle selection      */

#define TIMEDISP_DEFAULT        0    /* Standard on screen time display */
#define TIMEDISP_1              1    /* New on screen time display      */

#define PLAY_FULL_STOP_DEFAULT  0    /* Do first play when play from full stop */
#define PLAY_FULL_STOP_1        1    /* Play title 1 when play from full stop */

#define ZOOM_MODE_DEFAULT       0    /* continous cursor movement */
#define ZOOM_MODE_1             1    /* A cursor turns into movement when pushes it once,
                                      * and stops movement when pushes it once again */
#define CHAP_DISP_DEFAULT       0    /* Donot display chapter number in display bar
                                        if ptt search is prohibited */
#define CHAP_DISP_1             1    /* Display chapter number in display bar
                                        if ptt search is prohibited */

/* VCD Task Specific System Configurations */

/* define the timebar values we want to see */
#define TRACK_ALBUM_DISPLAY     0
#define SCENE_ALBUM_DISPLAY     1
#define TRACK_SCENE_DISPLAY     2

/* define what color to blank the screen in VCD aspect switches */
#define ASPECT_BLACK            0
#define ASPECT_BLUE             1
#define ASPECT_USER             2


/* End VCD Task Specific System Configurations */


/*
 * The following are the features or hardware which can be included
 * or selected to the system.
 *
 *         KARAOKE_TYPE             - Specify Karaoke chip
 *         DEFAULT_SYS_PARAM_TYPE   - Specify default system parameters
 *         PROGRAM_MODE             - Specify DVD program playback mode
 *         OSD_TYPE                 - Specify On Screen Display appearance
 *         AUDIO_MUTE_SELECT        - Specify master audio mute
 *         KEY_PLAY_INTERPRET_MODE  - Specify the function of PLAY key
 *         MULTIPLE_SYSTEM          - Specify multipe TV system
 *         TV_OUTPUT_DEFAULT        - Specify TV system in first power up
 *         SUBTITLE_MODE            - Specify on screen subtitle selection appearance
 *         TIMEDISP_MODE            - Specify on screen time display bar appearance
 *         ZOOM_MODE                - Specify zoom feature
 *
 *         LOADER_ERROR_RECOVER     - Enable loader error handling routine
 */

#define KARAOKE_TYPE                NO_KARAOKE
#define DEFAULT_SYS_PARAM_TYPE      DEFAULT_SYS_PARAM_0
#define PROGRAM_MODE                PROGRAM_MODE_2
#define OSD_TYPE                    OSD_2
#define AUDIO_MUTE_SELECT           AUDIO_MUTE_DEFAULT
#define AUTO_PWR_DOWN               FALSE
#define MULTIPLE_SYSTEM             MULTIPLE_SYSTEM_OFF
#define SUBTITLE_MODE               SUBTITLE_1
#define TIMEDISP_MODE               TIMEDISP_DEFAULT
#define DVD_AUDIO_DIV_CLK           384
#define VERSION_CHECK

#ifndef VDVD_PORT_TL_COMPILE
#define SET_PICTURE_MODE
#undef  SCRSAVER
#define SCRSAVER_TIME               12000
#endif

#define KEY_PLAY_INTERPRET_MODE     KEY_PLAY_INTERPRET_DEFAULT
#define TV_OUTPUT_DEFAULT           TV_OUTPUT_DEFAULT_NTSC
#define CONDITIONAL_RESUME_FROM_STOP
#define ZOOM_MODE                   ZOOM_MODE_1
#define LOADER_ERROR_RECOVER        TRUE
#define CHAP_DISP_MODE              CHAP_DISP_DEFAULT

#define DIGITAL_AUDIO               DIGITAL_AUDIO_DEFAULT
#define TRACK_DISPLAY_MODE          TRACK_ALBUM_DISPLAY
#define ASPECT_BLANK                ASPECT_USER
#define REPEAT_MODE                 REPEAT_MODE_DEFAULT
#define MAX_CLIPS_QUICKLOAD         25

/* Software Version Number:
 * Make sure using hex number (JC) */
#define VERSION_STRING      "Videon-Central SDK"
#define VERSION_DATE_STRING "11-28-2006"
#define VERSION_YEAR        0x2006
#define VERSION_MONTH       0x06
#define VERSION_DAY         0x28



#define false                   (0)
#define true                    (1)

#ifndef UNUSED_PARAM
#define UNUSED_PARAM(a) (void)a
#endif


#ifdef _MSC_VER

    // Work around for Microsoft Windows vs Linux string.h header file
    // Microsoft declares  _snprintf, not snprintf, and declares it in stdio.h
    // Microsft declares sprintf in stdio.h

    #include <stdio.h>

    #define snprintf    _snprintf

    #include <fcntl.h>
    #include <sys/stat.h>
    #include <io.h>


    #define FILE_HANDLE                         int

    #define FILE_MODE_READONLY                  _O_RDONLY
    #define FILE_MODE_WRITEONLY                 _O_WRONLY     
    #define FILE_MODE_READWRITE                 _O_RDWR
    #define FILE_MODE_APPEND                    _O_APPEND
    #define FILE_MODE_CREATE                    (_O_CREAT | _S_IREAD | _S_IWRITE)
    #define FILE_MODE_TRUNCATE                  _O_TRUNC
    #define FILE_MODE_BINARY                    _O_BINARY

    #define FILE_OPEN                           _open
    #define FILE_CREATE(filename)               _creat(filename, (_S_IREAD | _S_IWRITE) )
    #define FILE_CLOSE                          _close
    #define FILE_SEEK                           _lseeki64
    #define FILE_WRITE                          _write
    #define FILE_FLUSH                          _commit

#endif

#ifdef __CYGWIN__

    // Work around for CYGWIN vs Linux string.h header file
    // CYGWIN declares  snprintf in stdio.h
    
    #include <stdio.h>

    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>


    #define FILE_HANDLE                         int

    #define FILE_MODE_READONLY                  O_RDONLY
    #define FILE_MODE_WRITEONLY                 O_WRONLY     
    #define FILE_MODE_READWRITE                 O_RDWR
    #define FILE_MODE_APPEND                    O_APPEND
    #define FILE_MODE_CREATE                    O_CREAT
    #define FILE_MODE_TRUNCATE                  O_TRUNC
    #define FILE_MODE_BINARY                    0


    #define FILE_OPEN                           open
    #define FILE_CREATE(filename)               creat(filename,O_RDWR)
    #define FILE_CLOSE                          close
    #define FILE_SEEK                           lseek
    #define FILE_WRITE                          write
    #define FILE_FLUSH                          fsync


#endif

#if defined(__linux__) || defined(__LINUX__)

    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>


    #define FILE_HANDLE                         int

    #define FILE_MODE_READONLY                  O_RDONLY
    #define FILE_MODE_WRITEONLY                 O_WRONLY     
    #define FILE_MODE_READWRITE                 O_RDWR
    #define FILE_MODE_APPEND                    O_APPEND
    #define FILE_MODE_CREATE                    O_CREAT
    #define FILE_MODE_TRUNCATE                  O_TRUNC
    #define FILE_MODE_BINARY                    0


    #define FILE_OPEN                           open
    #define FILE_CREATE(filename)               creat(filename,O_RDWR)
    #define FILE_CLOSE                          close
    #define FILE_SEEK                           lseek
    #define FILE_WRITE                          write
    #define FILE_FLUSH                          fsync


#endif


#endif
