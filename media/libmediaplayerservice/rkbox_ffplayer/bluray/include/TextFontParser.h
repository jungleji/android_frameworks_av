#ifndef _RK_BLURAY_TEXTFONTPARSER_H_
#define _RK_BLURAY_TEXTFONTPARSER_H_

#include <stdio.h>


namespace RKBluray
{

#define TEXT_SUBTITLE_MAX 255

class TextFont
{
public:
    TextFont()
    {
        mSize = 0;
        mId = -1;
        mData = NULL;
    }

    ~TextFont()
    {
        if(mData != NULL)
        {
            delete[] mData;
        }
    }
    
public:
    int mSize;
    int mId;
    unsigned char* mData;
    
};


class TextFontParser
{
public:
    static TextFont* LoadFont(char*fontFile);
};   

}
#endif
