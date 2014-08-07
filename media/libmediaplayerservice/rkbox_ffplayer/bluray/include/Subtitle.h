#ifndef _RK_BLURAY_SUBTITLE_H_
#define _RK_BLURAY_SUBTITLE_H_

#include "PG.h"
#include "TextSubtitle.h"

namespace RKBluray
{


#define  MAX_DIALOG  2

class Subtitle
{
public:
    Subtitle();
    ~Subtitle();
public:
    int mComplete;                    // ����subtitle�Ƿ�����
    
    int mDialogCount;
    int mTotalDialog;

    PG* mPG;

    DialogPresentation*  mDialog;
    TextSubtitleDialogStyle*         mDialogStyle;
};

}
#endif
