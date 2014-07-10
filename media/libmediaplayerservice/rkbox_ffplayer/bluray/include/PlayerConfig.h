#ifndef _RKBLURAY_PLAYER_CONFIG_H_
#define _RKBLURAY_PLAYER_CONFIG_H_

namespace RKBluray{


typedef enum tagPE_IG_UI_MODEL
{
    PE_IG_UI_MODEL_ALWAYS_ON = 0,
    PE_IG_UI_MODEL_POPUP = 1
} PE_IG_UI_MODEL;
    
typedef struct _tagPE_ICONFIGURE_SPU_PALETTE
{
    //UCHAR palette[48];
    unsigned char palette[256*3]; /* support hd-dvd */
} PE_ICONFIGURE_SPU_PALETTE;
    
/* iConfigure Subpic state */
typedef enum
{
    SUBPIC_STATE_INVALID         = 0x00,
    SUBPIC_STATE_CREATED         = 0x01,
    SUBPIC_STATE_DECODING        = 0x02,
    SUBPIC_STATE_SHOWING         = 0x04,
    SUBPIC_STATE_HILIGHT_ENABLED = 0x08,
    SUBPIC_STATE_FORCED_DISABLED = 0x10
} PE_ICONFIGURE_SUBPIC_STATE;
    
/* iConfigure Audio Capabilities */
typedef enum
{
    AUDIO_CAPABLE_INVALID           = 0x00,
    AUDIO_CAPABLE_SURROUND          = 0x01,
    AUDIO_CAPABLE_LPCM_48_96        = 0x02,
    AUDIO_CAPABLE_LPCM_192          = 0x04,
    AUDIO_CAPABLE_AC3               = 0x08,
    AUDIO_CAPABLE_DTS               = 0x10,
    AUDIO_CAPABLE_DTS_HD            = 0x20,
    AUDIO_CAPABLE_DD_PLUS           = 0x40,
    AUDIO_CAPABLE_DOLBY_LOSSLESS    = 0x80
} PE_ICONFIGURE_AUDIO_CAPABILITY;
    
/* iConfigure TextST Capabilities */
typedef enum
{
    TEXTST_CAPABLE_INVALID              = 0x0000,
    TEXTST_CAPABLE_UNSUPPORTED_LANG     = 0x0001,
    TEXTST_CAPABLE_USER_STYLE_CHANGE    = 0x0002,
    TEXTST_CAPABLE_TEXT_FLOW_ALIGN      = 0x0004,
    TEXTST_CAPABLE_OUTLINE_BORDER       = 0x0008,
    TEXTST_CAPABLE_INLINE_STYLE         = 0x0010,
    TEXTST_CAPABLE_BIG5                 = 0x0020,
    TEXTST_CAPABLE_CN_GB                = 0x0040,
    TEXTST_CAPABLE_GB18030_2000         = 0x0080,
    TEXTST_CAPABLE_EUC_KR               = 0x0100,
    TEXTST_CAPABLE_SHIFT_JIS            = 0x0200
} PE_ICONFIGURE_TEXTST_CAPABILITY;
    
/* iConfigure Player Version */
typedef enum
{
    PLAYER_VERSION_INVALID          = 0x00,
    PLAYER_VERSION_AUDIO_VIDEO      = 0x01,
    PLAYER_VERSION_AUDIO            = 0x02,
    PLAYER_VERSION_DUAL_INTERACTION = 0x04
} PE_ICONFIGURE_PLAYER_VERSION;


class RKPlayerConfig
{
public:
    int getAudioCapability(unsigned long* capability);
    int getTextSTCapability(unsigned long* capability);
};


}
#endif
