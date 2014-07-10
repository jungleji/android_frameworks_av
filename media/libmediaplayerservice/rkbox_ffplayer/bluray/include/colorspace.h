#ifndef _BLURAY_RGB_H_
#define _BLURAY_RGB_H_
#include <math.h>

namespace RKBluray 
{
//#define RGBA(r,g,b,a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))
//#define RGBA_A(color) ((color>>24)& 0xff)
//#define RGBA_R(color) ((color>>16)& 0xff)
//#define RGBA_G(color) ((color>>8)& 0xff)
//#define RGBA_B(color) (color& 0xff)
/*
#define RGBA(r,g,b,a) (((r) << 24) | ((g) << 16) | (b << 8) | a)
#define RGBA_A(color) (color & 0xff)
#define RGBA_R(color) ((color>>24)& 0xff)
#define RGBA_G(color) ((color>>16)& 0xff)
#define RGBA_B(color) ((color>>8) & 0xff)
*/
#define RGBA_A(color) ((color>>24)& 0xff)
#define RGBA_R(color) ((color>>16)& 0xff)
#define RGBA_G(color) ((color>>8) & 0xff)
#define RGBA_B(color) (color & 0xff)


    
#define SCALEBITS 10
#define ONE_HALF  (1 << (SCALEBITS - 1))
#define FIX(x)    ((int) ((x) * (1<<SCALEBITS) + 0.5))
#define POSITIVE(x) ( x > 0 ? x : 0)
    
#define YUV_TO_RGB1_CCIR(cb1, cr1)\
    {\
        cb = (cb1) - 128;\
        cr = (cr1) - 128;\
        r_add = FIX(1.40200*255.0/224.0) * cr + ONE_HALF;\
        g_add = - FIX(0.34414*255.0/224.0) * cb - FIX(0.71414*255.0/224.0) * cr + \
                ONE_HALF;\
        b_add = FIX(1.77200*255.0/224.0) * cb + ONE_HALF;\
    }
    
#define YUV_TO_RGB2_CCIR(r, g, b, y1)\
    {\
        y = ((y1) - 16) * FIX(255.0/219.0);\
        r = cm[(y + r_add) >> SCALEBITS];\
        g = cm[(y + g_add) >> SCALEBITS];\
        b = cm[(y + b_add) >> SCALEBITS];\
    }

#define YUV_TO_RGB0(y,cb,cr)\
{\
	r  = POSITIVE(floor(1.164*y + 1.793*cr - 247.628));\
	g = POSITIVE(floor(1.164*y -0.213*cb - 0.533*cr + 77.364));\
	b = POSITIVE(floor(1.164*y +2.112*cb - 288.46));\
}
    
#define YUV_TO_RGB1(cb1, cr1)\
    {\
        cb = (cb1) - 128;\
        cr = (cr1) - 128;\
        r_add = FIX(1.40200) * cr + ONE_HALF;\
        g_add = - FIX(0.34414) * cb - FIX(0.71414) * cr + ONE_HALF;\
        b_add = FIX(1.77200) * cb + ONE_HALF;\
    }
    
#define YUV_TO_RGB2(r, g, b, y1)\
    {\
        y = (y1) << SCALEBITS;\
        r = cm[(y + r_add) >> SCALEBITS];\
        g = cm[(y + g_add) >> SCALEBITS];\
        b = cm[(y + b_add) >> SCALEBITS];\
    }
    
#define Y_CCIR_TO_JPEG(y)\
     cm[((y) * FIX(255.0/219.0) + (ONE_HALF - 16 * FIX(255.0/219.0))) >> SCALEBITS]
    
#define Y_JPEG_TO_CCIR(y)\
     (((y) * FIX(219.0/255.0) + (ONE_HALF + (16 << SCALEBITS))) >> SCALEBITS)
    
#define C_CCIR_TO_JPEG(y)\
     cm[(((y) - 128) * FIX(127.0/112.0) + (ONE_HALF + (128 << SCALEBITS))) >> SCALEBITS]
    
    /* NOTE: the clamp is really necessary! */
    static inline int C_JPEG_TO_CCIR(int y) {
        y = (((y - 128) * FIX(112.0/127.0) + (ONE_HALF + (128 << SCALEBITS))) >> SCALEBITS);
        if (y < 16)
            y = 16;
        return y;
    }
    
    
#define RGB_TO_Y(r, g, b) \
    ((FIX(0.29900) * (r) + FIX(0.58700) * (g) + \
      FIX(0.11400) * (b) + ONE_HALF) >> SCALEBITS)
    
#define RGB_TO_U(r1, g1, b1, shift)\
    (((- FIX(0.16874) * r1 - FIX(0.33126) * g1 +         \
         FIX(0.50000) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)
    
#define RGB_TO_V(r1, g1, b1, shift)\
    (((FIX(0.50000) * r1 - FIX(0.41869) * g1 -           \
       FIX(0.08131) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)
    
#define RGB_TO_Y_CCIR(r, g, b) \
    ((FIX(0.29900*219.0/255.0) * (r) + FIX(0.58700*219.0/255.0) * (g) + \
      FIX(0.11400*219.0/255.0) * (b) + (ONE_HALF + (16 << SCALEBITS))) >> SCALEBITS)
    
#define RGB_TO_U_CCIR(r1, g1, b1, shift)\
    (((- FIX(0.16874*224.0/255.0) * r1 - FIX(0.33126*224.0/255.0) * g1 +         \
         FIX(0.50000*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)
    
#define RGB_TO_V_CCIR(r1, g1, b1, shift)\
    (((FIX(0.50000*224.0/255.0) * r1 - FIX(0.41869*224.0/255.0) * g1 -           \
       FIX(0.08131*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)


    
/* pixel operations */
#define MAX_NEG_CROP 1024
//extern uint8_t ff_cropTbl[256 + 2 * MAX_NEG_CROP];
    
    
    //uint8_t ff_cropTbl[256 + 2 * MAX_NEG_CROP] = {0, };
    
//#define cm (ff_cropTbl + MAX_NEG_CROP)

#ifndef RGBA_IN
#define RGBA_IN(r, g, b, a, s)\
{\
    unsigned int v = ((const unsigned int *)(s))[0];\
    a = (v >> 24) & 0xff;\
    r = (v >> 16) & 0xff;\
    g = (v >> 8) & 0xff;\
    b = v & 0xff;\
}
#endif

#ifndef YUVA_IN
#define YUVA_IN(y, u, v, a, s, pal)\
{\
    unsigned int val = ((const unsigned int *)(pal))[*(const unsigned char*)(s)];\
    a = (val >> 24) & 0xff;\
    y = (val >> 16) & 0xff;\
    u = (val >> 8) & 0xff;\
    v = val & 0xff;\
}
#endif

#ifndef YUVA_OUT
#define YUVA_OUT(d, y, u, v, a)\
{\
    ((unsigned int *)(d))[0] = (a << 24) | (y << 16) | (u << 8) | v;\
}
#endif

#ifndef ALPHA_BLEND
#define ALPHA_BLEND(a, oldp, newp, s)\
((((oldp << s) * (255 - (a))) + (newp * (a))) / (255 << s))
#endif



}

#endif
