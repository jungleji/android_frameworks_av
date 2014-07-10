/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CC_IMAGE_H__
#define __CC_IMAGE_H__

#define CCIMAGE_CLASS "xming.CCImage"

typedef unsigned char uchar8;
struct ARGB32{ 
	uchar8  b,g,r,a;
};  
struct ColorRegion{ 
  ARGB32* pdata;
  long byte_width;
  long width;
  long height;
};

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int UINT;

#define RGB32(r,g,b)    ( ((DWORD)(BYTE)r)|((DWORD)((BYTE)g)<<8)|((DWORD)((BYTE)b)<<16) )
#define GetRValue(RGBColor)    (BYTE) (RGBColor)
#define GetGValue(RGBColor)    (BYTE) (((DWORD)RGBColor) >> 8)
#define GetBValue(RGBColor)    (BYTE) (((DWORD)RGBColor) >> 16)

typedef DWORD COLORREF;

#define min(X,Y)  ((X)<(Y)?(X):(Y))
#define max(A,B) ((A)>(B)?(A):(B))

class  CCImage{
public:
    typedef enum
    {
        kFmtJpg = 0,
        kFmtPng,
        kFmtRawData,
        kFmtUnKnown
    }EImageFormat;

    //CCImage();
    CCImage(int screenwidth, int screenheight);
    ~CCImage();

    bool initWithImageFile(const char * strPath, EImageFormat eFmt = kFmtUnKnown);
    bool initWithImageData(void *pData, int nDataLen, EImageFormat eFmt = kFmtUnKnown,
                           int nWidth = 0,int nHeight = 0,int nBitsPerComponent = 8);
    bool initWithRawData(void *pData, int nDatalen, int nWidth, int nHeight, int nBitsPerComponent);
    void auto_scale();
    void auto_scale(int width,int height) {AutoScaleForScreen(width, height);}
    unsigned char *   getData(){ return m_pData; }
    int getDataLen(){ return m_nBufferWidth * m_nHeight; }
    bool hasAlpha(){ return m_bHasAlpha; }
    bool isPremultipliedAlpha(){ return m_bPreMulti; }
    int getWidth(){return m_nWidth;}
    int getHeight(){return m_nHeight;}
    int getBitsPerComponent(){return m_nBitsPerComponent;}
    int getBufferwidth(){return m_nBufferWidth;}

//protected:
public:
    int AutoScaleForScreen(int width, int height);
    unsigned char* getFileData(const char* pszFileName, const char* pszMode, unsigned long *pSize);
    bool _initWithJpgData(void *pData, int nDatalen);
    bool _initWithPngData(void *pData, int nDatalen);

    unsigned char *m_pData;
    bool m_bHasAlpha;
    bool m_bPreMulti;
    int m_nWidth;
    int m_nHeight;
    int m_nBitsPerComponent;
    int m_nBufferWidth;

    int mScreenWidth;
    int mScreenHeight;

private:
    // noncopyable
    CCImage(const CCImage&    rImg);
    CCImage & operator=(const CCImage&);
};

#endif    // __CC_IMAGE_H__
