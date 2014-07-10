#ifndef _BLURAY_REGISTER
#define _BLURAY_REGISTER


namespace RKBluray{


#if 0
PSR
    1,           /*     PSR0:  Interactive graphics stream number */
    0xff,        /*     PSR1:  Primary audio stream number */
    0x0fff0fff,  /*     PSR2:  PG TextST stream number and PiP PG stream number*/  // 最高位表示是否显示PG
    1,           /*     PSR3:  Angle number */
    0xffff,      /*     PSR4:  Title number */
    0xffff,      /*     PSR5:  Chapter number */
    0,           /*     PSR6:  PlayList ID */
    0,           /*     PSR7:  PlayItem ID */
    0,           /*     PSR8:  Presentation time */
    0,           /*     PSR9:  Navigation timer */
    0xffff,      /*     PSR10: Selected button ID */
    0,           /*     PSR11: Page ID */
    0xff,        /*     PSR12: User style number */
    0xff,        /* PS: PSR13: User age */
    0xffff,      /*     PSR14: Secondary audio stream number and secondary video stream number */
    0xffff,      /* PS: PSR15: player capability for audio */
    0xffffff,    /* PS: PSR16: Language code for audio */
    0xffffff,    /* PS: PSR17: Language code for PG and Text subtitles */
    0xffffff,    /* PS: PSR18: Menu description language code */
    0xffff,      /* PS: PSR19: Country code */
    0x07,        /* PS: PSR20: Region code */ /* 1 - A, 2 - B, 4 - C */
    0,           /*     PSR21 */
    0,           /*     PSR22 */
    0,           /*     PSR23: Display capbility: 0: 2D, 1: 3D*/
    0,           /*     PSR24 */
    0,           /*     PSR25 */
    0,           /*     PSR26 */
    0,           /*     PSR27 */
    0,           /*     PSR28 */
    0x03,        /* PS: PSR29: player capability for video */
    0x1ffff,     /* PS: PSR30: player capability for text subtitle */

    /* Player Profile/Version Number
    * b19~b16: profile(0,1,3,5)
    * b15~b0: version(0x0200)
    */
    0x080200,    /* PS: PSR31: Player profile and version */
    0,           /*     PSR32 */
    0,           /*     PSR33 */
    0,           /*     PSR34 */
    0,           /*     PSR35 */
    0xffff,      /*     PSR36: backup PSR4 */
    0xffff,      /*     PSR37: backup PSR5 */
    0,           /*     PSR38: backup PSR6 */
    0,           /*     PSR39: backup PSR7 */
    0,           /*     PSR40: backup PSR8 */
    0,           /*     PSR41: */
    0xffff,      /*     PSR42: backup PSR10 */
    0,           /*     PSR43: backup PSR11 */
    0xff,        /*     PSR44: backup PSR12 */
    0,           /*     PSR45: */
    0,           /*     PSR46: */
    0,           /*     PSR47: */
    0xffffffff,  /* PS: PSR48: Characteristic text caps */
    0xffffffff,  /* PS: PSR49: Characteristic text caps */
    0xffffffff,  /* PS: PSR50: Characteristic text caps */
    0xffffffff,  /* PS: PSR51: Characteristic text caps */
    0xffffffff,  /* PS: PSR52: Characteristic text caps */
    0xffffffff,  /* PS: PSR53: Characteristic text caps */
    0xffffffff,  /* PS: PSR54: Characteristic text caps */
    0xffffffff,  /* PS: PSR55: Characteristic text caps */
    0xffffffff,  /* PS: PSR56: Characteristic text caps */
    0xffffffff,  /* PS: PSR57: Characteristic text caps */
    0xffffffff,  /* PS: PSR58: Characteristic text caps */
    0xffffffff,  /* PS: PSR59: Characteristic text caps */
    0xffffffff,  /* PS: PSR60: Characteristic text caps */
    0xffffffff,  /* PS: PSR61: Characteristic text caps */
    /* 62-95:   reserved */
    /* 96-111:  reserved for BD system use */
    /* 112-127: reserved */
#endif


/**
 * Player Status Register Numbers
 */
#define PLAYCTRL_PSR_IG_STN             (0)
#define PLAYCTRL_PSR_AUDIO_STN          (1)
#define PLAYCTRL_PSR_PG_AND_ST_STN      (2)
#define PLAYCTRL_PSR_ANGLE              (3)
#define PLAYCTRL_PSR_TITLE              (4)
#define PLAYCTRL_PSR_CHAPTER            (5)
#define PLAYCTRL_PSR_PLAYLIST           (6)
#define PLAYCTRL_PSR_PLAYITEM           (7)
#define PLAYCTRL_PSR_PRES_TIME          (8)
#define PLAYCTRL_PSR_NVTIMER            (9)
#define PLAYCTRL_PSR_SEL_BUTTON         (10)
#define PLAYCTRL_PSR_MENU_PAGE          (11)
#define PLAYCTRL_PSR_SEL_STYLE          (12)
#define PLAYCTRL_PSR_PARENTAL           (13)
#define PLAYCTRL_PSR_SEC_AUD_VID_STN    (14)
#define PLAYCTRL_PSR_AUDIO_CAPABILITY   (15)
#define PLAYCTRL_PSR_AUDIO_LANG         (16)
#define PLAYCTRL_PSR_PG_AND_ST_LANG     (17)
#define PLAYCTRL_PSR_MENU_LANG          (18)
#define PLAYCTRL_PSR_COUNTRY            (19)
#define PLAYCTRL_PSR_REGION             (20)
#define PLAYCTRL_PSR_OUTPUT_PREFER      (21)
#define PLAYCTRL_PSR_3D_STATUS          (22)
#define PLAYCTRL_PSR_DISPLAY_CAP        (23)
#define PLAYCTRL_PSR_3D_CAP             (24)

#define PLAYCTRL_PSR_VIDEO_CAPABILITY   (29)
#define PLAYCTRL_PSR_TEXT_CAPABILITY    (30)
#define PLAYCTRL_PSR_PROFILE_VERSION    (31)

#define PLAYCTRL_PSR_APP_LAYER_0        (102)
#define PLAYCTRL_PSR_APP_LAYER_1        (103)
#define PLAYCTRL_PSR_APP_LAYER_2        (104)

/**
 * Player Status Register Default Vales
 */
#define PSR_IG_STN_DEFAULT              0x00000001
#define PSR_AUDIO_STN_DEFAULT           0x000000FF
#define PSR_PG_AND_ST_STN_DEFAULT       0x0FFF0FFF
#define PSR_ANGLE_DEFAULT               0x00000001
#define PSR_TITLE_DEFAULT               0x0000FFFF
#define PSR_CHAPTER_DEFAULT             0x0000FFFF
#define PSR_PLAYLIST_DEFAULT            0x00000000
#define PSR_PLAYITEM_DEFAULT            0x00000000
#define PSR_PRES_TIME_DEFAULT           0x00000000
#define PSR_NVTIMER_DEFAULT             0x00000000
#define PSR_SEL_BUTTON_DEFAULT          0x0000FFFF
#define PSR_MENU_PAGE_DEFAULT           0x00000000
#define PSR_SEL_STYLE_DEFAULT           0x000000FF
#define PSR_SEC_AUD_VID_STN_DEFAULT     0x0000FFFF

#define PSR_APP_LAYER_0_DEFAULT         0x00000000
#define PSR_APP_LAYER_1_DEFAULT         0x00000000
#define PSR_APP_LAYER_2_DEFAULT         0x00000000

#define PSR_PROFILE_VERSION5            0x130240//0x050200



class BlurayRegisters
{
    public:
        BlurayRegisters();
        ~BlurayRegisters();
        unsigned int getRegistersValue(unsigned long oprand);
        int setGPRsValue(int index,unsigned int value);
        int setPSRsValue(int index,unsigned int value);
        int getPSRsValue(int index,unsigned int* value);
        int getGPRsValue(int index,unsigned int* value);
        void PSRsBackup();
        void PSRsResetBackup();
        void PSRsLoadBackup();
    private:
        // player status registers
        unsigned int* mPSRs;
        // general purpose registers
        unsigned int* mPGRs;

        void PSRsInit();
   //     int getPSRsValue(int index,unsigned int* value);
   //     int getGPRsValue(int index,unsigned int* value);
        
};


}

#endif

