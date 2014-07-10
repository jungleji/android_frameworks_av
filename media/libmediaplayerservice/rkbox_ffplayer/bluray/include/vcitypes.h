/* vim:set ts=4 sw=4 et: */
/*****************************************************************************
******************************************************************************
**                                                                          **
**  Copyright (c) 2003, 2004, 2005 Videon Central, Inc.                     **
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
 * vcitypes.h
 *
 * Common types for cross-platform development.
 *
 * @author
 * Art Shipkowski
 *
 * @remark
 * None
 *
 * $Id: vcitypes.h,v 1.39 2006/11/13 14:53:47 ctusar Exp $
 */
#if !defined(VCITYPES_H)
#define VCITYPES_H


/*****************************************************************************
******************************************************************************
**                                                                          **
**  Compiler / Platform:                                                    **
**    Visual C++                                                            **
**                                                                          **
**  Author:                                                                 **
**    Jared Arteca                                                          **
**                                                                          **
**  Maintainer:                                                             **
**    Jared Arteca                                                          **
**                                                                          **
**  Notes:                                                                  **
**    none                                                                  **
**                                                                          **
**  Revision History:                                                       **
**    08/03/00  JCA    File origination                                     **
**    10/03/00  JCA    Fixes for stdafx.h being included                    **
**                                                                          **
******************************************************************************
*****************************************************************************/
#ifdef _MSC_VER

/*****************************************************************************
*                                                                            *
* Signed Data Types                                                          *
*                                                                            *
*****************************************************************************/
#ifndef _WINDOWS_ 
 #ifndef _WINNT_
  typedef signed char         CHAR;                 /* 8  bits - signed character */
  typedef signed short        WCHAR;                /* 16 bits - signed character (Unicode) */
 #endif
#endif

typedef signed short        SHORT;                /* 16 bits - signed integer */
typedef signed long         LONG;                 /* 32 bits - signed integer */
typedef signed _int64       LONGLONG;             /* 64 bits - signed integer */

typedef signed char         int8;
typedef signed short        int16;
typedef signed long         int32;
typedef signed _int64       int64;

/*****************************************************************************
*                                                                            *
* Unsigned Data Types                                                        *
*                                                                            *
*****************************************************************************/

typedef unsigned char         BYTE;               /* 8  bits - unsigned integer */
typedef unsigned char         UCHAR;              /* 8  bits - unsigned integer */
typedef unsigned short        USHORT;             /* 16 bits - unsigned integer */
typedef unsigned long         ULONG;              /* 32 bits - unsigned integer */
typedef unsigned _int64       ULONGLONG;          /* 64 bits - unsigned integer */

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned long       uint32;
typedef unsigned _int64     uint64;

/*****************************************************************************
*                                                                            *
* Floating Point Data Types                                                  *
*                                                                            *
*****************************************************************************/
typedef float                 FLOAT;              /* 32 bits - floating point (real) */
typedef double                DOUBLE;             /* 64 bits - double precision */

/*****************************************************************************
*                                                                            *
* Boolean Data Types and Constants                                           *
*                                                                            *
*****************************************************************************/
typedef unsigned char         BOOLEAN;            /* 8 bits - boolean */

#ifndef true                                      /* true = 1 */
#define true (1)
#endif

#ifndef TRUE                                      /* TRUE = 1 */
#define TRUE (1)
#endif

#ifndef false                                     /* false = 0 */
#define false (0)
#endif

#ifndef FALSE                                     /* FALSE = 0 */
#define FALSE (0)
#endif

/*****************************************************************************
*                                                                            *
* Void Pointer Data Type                                                     *
*                                                                            *
*****************************************************************************/
typedef void *                PVOID;              /* 32 bits - void pointer */

/*****************************************************************************
*                                                                            *
* String Data Type                                                           *
* NULL terminated string made up of an array of type CHAR                    *
*                                                                            *
*****************************************************************************/
typedef CHAR *                STR;                /* 8*(strlen+1) bits */

typedef BYTE *                PBYTE;

/*****************************************************************************
*                                                                            *
* Error Code Status Structure                                                *
*                                                                            *
*****************************************************************************/
typedef unsigned long         ERR_CODE;           /* 32 bits - unsigned integer */

#define ISEQUAL(x, y) (x) == (y)

#define __attribute__(x)

#define MAX(x, y)                       (((x) > (y)) ? (x) : (y))
#define MIN(x, y)                       (((x) < (y)) ? (x) : (y))

#endif /* _MSC_VER */


/*****************************************************************************
******************************************************************************
**                                                                          **
**  Compiler / Platform:                                                    **
**    Green Hills / MIPS R3000                                              **
**                                                                          **
**  Author:                                                                 **
**    Micah Willy                                                           **
**                                                                          **
**  Maintainer:                                                             **
**    Micah Willy                                                           **
**                                                                          **
**  Notes:                                                                  **
**    1)  The L64030's CPU core, the TinyRISC EZ4102 is MIPS R3000          **
**        compliant, the long long, 64 bit storage types may need compiler  **
**        settings to enable 64 bit emulation or register handling.         **
**    2)  The L64030's CPU core, the TinyRISC EZ4102 is MIPS R3000          **
**        compliant, there is most likely not a floating math co-processor  **
**        so the -fsoft switch in Green Hills must be used to enable        **
**        software floating point emulation.  Green Hills / MIPS supports   **
**        IEEE 754 floating point formatting.                               **
**                                                                          **
**  Revision History:                                                       **
**    08/07/00  MBW   File origination                                      **
**                                                                          **
******************************************************************************
*****************************************************************************/
#ifdef __ghs

/*****************************************************************************
*                                                                            *
* Type Definitions                                                           *
*                                                                            *
*****************************************************************************/
/* -- Signed Types -- */                /*  BITS  DESCRIPTION     PREFIX */
typedef signed char         CHAR;       /*  8     signed character  c    */
typedef signed short        WCHAR;      /*  16    signed character  wc   */
typedef signed short        SHORT;      /*  16    signed integer    s    */
typedef signed long         LONG;       /*  32    signed integer    l    */
#if 0
typedef signed long long    LONGLONG;   /*  64    signed integer    ll   */
#endif

/* -- Unsigned Types -- */
typedef unsigned char       BYTE;       /*  8     unsigned integer  b    */
typedef unsigned char       UCHAR;      /*  8     unsigned integer  uc   */
typedef unsigned short      USHORT;     /*  16    unsigned integer  us   */
typedef unsigned long       ULONG;      /*  32    unsigned integer  ul   */
#if 0
typedef unsigned long long  ULONGLONG;  /*  64    unsigned integer  ull  */
#endif

/* -- Conditional Returns and Error Codes -- */
typedef unsigned char       BOOLEAN;    /*  8     Bool(T 1, F 0)    f    */
typedef unsigned long       ERR_CODE;   /*  32    unsigned integer  ec   */

/* -- Floating Point Types -- */
typedef float               FLOAT;      /*  32    floating point    fl   */
typedef double              DOUBLE;     /*  64    double-precision  d    */

/* -- Pointer Types -- */
typedef void *              PVOID;      /*  32    void pointer      pv   */
typedef signed char *       STR;        /*  8 x (strlen + 1)        str  */

/*****************************************************************************
*                                                                            *
* Standard Definitions                                                       *
*                                                                            *
*****************************************************************************/
/* -- Standard Definitions ( TRUE, FALSE, SUCCESS, Error Codes ) -- */
#define TRUE  0x01
#define FALSE 0x00
#define DUMMY_ERR_CODE  0xfeedf00d

/*****************************************************************************
*                                                                            *
*  Macros                                                                    *
*                                                                            *
*****************************************************************************/
/* -- Data Formatting Macros ( HIBYTE, LOBYTE, n'at ) -- */

#endif /* __ghs */


/*****************************************************************************
******************************************************************************
**                                                                          **
**  Compiler / Platform:                                                    **
**    Trimedia                                                              **
**                                                                          **
**  Author:                                                                 **
**    Arthur Shipkowski                                                     **
**                                                                          **
**  Maintainer:                                                             **
**    Arthur Shipkowski                                                     **
**                                                                          **
**  Notes:                                                                  **
**    No long long (64-bit) types available AFAIK.                          **
**                                                                          **
**  Revision History:                                                       **
**    08/22/00  AES   File origination                                      **
**                                                                          **
******************************************************************************
*****************************************************************************/
#ifdef __TCS__

/*********************/
/* Value-based Types */
/*********************/
typedef signed char CHAR;                         /* 8  bits - signed character */
typedef signed short WCHAR;                       /* 16 bits - signed character (Unicode) */
typedef signed short SHORT;                       /* 16 bits - signed integer */
typedef signed long LONG;                         /* 32 bits - signed integer */
/* No long long type available AFAIK */
typedef unsigned char BYTE;                       /* 8  bits - unsigned integer */
typedef unsigned char UCHAR;                      /* 8  bits - unsigned integer */
typedef unsigned short USHORT;                    /* 16 bits - unsigned integer */
typedef unsigned long ULONG;                      /* 32 bits - unsigned integer */
/* No unsigned long long type available AFAIK either */
typedef float FLOAT;                              /* 32 bits - floating point (real) */
typedef double DOUBLE;                            /* 64 bits - double precision */
typedef unsigned long ERR_CODE;                   /* 32 bits - unsigned integer */
typedef unsigned char BOOLEAN;                    /* 8  bits - boolean */

/* Boolean values -- I think these are predefined, but it's best to be sure */
#ifndef TRUE                                      /* TRUE = 1 */
#define TRUE (1)
#endif

#ifndef FALSE                                     /* FALSE = 0 */
#define FALSE (0)
#endif

/*****************/
/* Pointer Types */
/*****************/
typedef void *PVOID;                              /* 32 bits - void pointer */
typedef CHAR *STR;                                /* 8*(strlen+1) bits (Null Terminated)*/

#endif /* __TCS__ */


/* ---------------------------------------------------------------------------
 * Compiler / Platform:
 * Keil C51 (8051)
 *
 * Author:
 * Cory T. Tusar
 *
 * Maintainer:
 * Cory T. Tusar
 *
 * Notes:
 * This compiler does not support 64-bit data types.  It's happiest with
 * 8-bit unsigned stuff.
 */
#if defined(__C51__)

#if 0  /* This is ANSI C (stdarg.h), and should not be redefined here. */
#if defined(NULL)
#warning "Redefining NULL in vcitypes.h"
#undef NULL
#endif  /* NULL */
#define NULL                            ((void *)0)
#endif

#if defined(TRUE)
#warning "Redefining TRUE in vcitypes.h"
#undef TRUE
#endif  /* TRUE */
#define TRUE                            (1 == 1)


#if defined(FALSE)
#warning "Redefining FALSE in vcitypes.h"
#undef FALSE
#endif  /* FALSE */
#define FALSE                           (1 == 0)


#undef DBG_PRINT
#if defined(DEBUG)
#define DBG_PRINT(x)                    { uartPrintf x; }
#else
#define DBG_PRINT(x)                    /##/ x
#endif  /* DEBUG */


/**
 * @def MIN(x, y)
 * Evaluates to the minimum of x and y.
 */
/**
 * @def MAX(x, y)
 * Evaluates to the maximum of x and y.
 */
#define MIN(x, y)                       (((x) < (y)) ? (x) : (y))
#define MAX(x, y)                       (((x) > (y)) ? (x) : (y))

#define NIBBLESWAP(x)                   ((x << 4) | (x >> 4))
#define BYTESWAP(x)                     ((x << 8) | (x >> 8))
#define WORDSWAP(x)                     ((x << 16) | (x >> 16))

#define uint16_high_byte_get(x)         ((uint8)(x >> 8))
#define uint16_low_byte_get(x)          ((uint8)x)

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef signed char int8;
typedef signed short int16;
typedef signed long int32;

typedef signed char CHAR;
typedef signed short WCHAR;
typedef signed short SHORT;
typedef signed long LONG;

typedef unsigned char BYTE;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;

typedef float FLOAT;
typedef double DOUBLE;
typedef unsigned long ERR_CODE;
typedef bit BOOLEAN;

typedef void *PVOID;
typedef char *STR;

#endif  /* __C51__ */


/* ---------------------------------------------------------------------------
 * Compiler / Platform:
 * SDCC MCS51 (8051)
 *
 * Author:
 * Cory T. Tusar
 *
 * Maintainer:
 * Cory T. Tusar
 *
 * Notes:
 * This compiler does not support 64-bit data types.  It's happiest with
 * 8-bit unsigned stuff.
 */
#if (defined(SDCC) && defined(SDCC_mcs51))

#if 0  /* This is ANSI C (stdarg.h), and should not be redefined here. */
#if defined(NULL)
#warning "Redefining NULL in vcitypes.h"
#undef NULL
#endif  /* NULL */
#define NULL                            ((void *)0)
#endif

#if defined(TRUE)
#warning "Redefining TRUE in vcitypes.h"
#undef TRUE
#endif  /* TRUE */
#define TRUE                            (1 == 1)

#if defined(FALSE)
#warning "Redefining FALSE in vcitypes.h"
#undef FALSE
#endif  /* FALSE */
#define FALSE                           (1 == 0)


//#define DBG_PRINT(x) {printf(x)}
//printf("%s: %d: ", __FILE__, __LINE__); printf x; printf("\n");


/**
 * @def MIN(x, y)
 * Evaluates to the minimum of x and y.
 */
/**
 * @def MAX(x, y)
 * Evaluates to the maximum of x and y.
 */
#define MIN(x, y)                       (((x) < (y)) ? (x) : (y))
#define MAX(x, y)                       (((x) > (y)) ? (x) : (y))

#define NIBBLESWAP(x)                   ((x << 4) | (x >> 4))
#define BYTESWAP(x)                     ((x << 8) | (x >> 8))
#define WORDSWAP(x)                     ((x << 16) | (x >> 16))


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef signed char int8;
typedef signed short int16;
typedef signed long int32;

typedef signed char CHAR;
typedef signed short WCHAR;
typedef signed short SHORT;
typedef signed long LONG;

typedef unsigned char BYTE;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;

typedef float FLOAT;
typedef unsigned long ERR_CODE;
typedef unsigned char BOOLEAN;

typedef void *PVOID;
typedef char *STR;

#endif  /* SDCC && SDCC_mcs51 */


/*****************************************************************************
******************************************************************************
**                                                                          **
**  Compiler / Platform:                                                    **
**    vxWorks / GNUsparc                                                    **
**                                                                          **
**  Author:                                                                 **
**    Terry Corl                                                            **
**                                                                          **
**  Maintainer:                                                             **
**    Terry Corl                                                            **
**                                                                          **
**  Notes:                                                                  **
**    1)  vxWorks.h must precede this file in being #included!              **
**    2)  This compiler does not support 64-bit data types.                 **
**                                                                          **
**  Revision History:                                                       **
**    08/22/00  TC    File origination                                      **
**    09/20/00  CTT   Fixed backwards typedefs                              **
**                                                                          **
******************************************************************************
*****************************************************************************/
#ifdef __INCvxWorksh

/* defines for vxWorks tornado environment */
typedef char                  CHAR;               /* 8 bit */
typedef unsigned char         BYTE;               /* 8 bit */
typedef unsigned char         UCHAR;              /* 8 bit */
typedef unsigned char         BOOLEAN;            /* 8 bit */
typedef unsigned short        USHORT;             /* 16 bit */
typedef short                 WCHAR;              /* 16 bit */
typedef short                 SHORT;              /* 16 bit */
typedef int                   LONG ;              /* 32 bit */
typedef unsigned int          ULONG;              /* 32 bit */
typedef void *                PVOID;              /* 32 bit */
typedef unsigned int          ERR_CODE;           /* 32 bit */
typedef float                 FLOAT;
typedef double                DOUBLE;

/*  64 bit not defined for this compiler
typedef unsigned long long    ULONGLONG;
typedef long long             LONGLONG;
*/

#endif  /* __INCvxWorksh */


#if defined(__linux__) || defined(__LINUX__)
/*****************************************************************************
******************************************************************************
**                                                                          **
**  Compiler / Platform:                                                    **
**    uClinux                                                               **
**                                                                          **
**  Author:                                                                 **
**    Arthur Shipkowski                                                     **
**                                                                          **
**  Maintainer:                                                             **
**    Arthur Shipkowski                                                     **
**                                                                          **
**  Notes:                                                                  **
**    64-bit values only available on some platforms.                       **
**                                                                          **
**  Revision History:                                                       **
**    03/14/02  AES   File origination                                      **
**                                                                          **
******************************************************************************
*****************************************************************************/

#if 0  /* This is ANSI C (stdarg.h), and should not be redefined here. */
#if defined(NULL)
#warning "Redefining NULL in vcitypes.h"
#undef NULL
#endif  /* NULL */
#define NULL                            ((void *)0)
#endif

#if defined(TRUE)
#warning "Redefining TRUE in vcitypes.h"
#undef TRUE
#endif  /* TRUE */
#define TRUE                            (1 == 1)

#if defined(FALSE)
#warning "Redefining FALSE in vcitypes.h"
#undef FALSE
#endif  /* FALSE */
#define FALSE                           (1 == 0)


#include <stdio.h>
#include <string.h>
#define DBG_PRINT(x) \
do { \
    char const * const _s = strrchr(__FILE__, '/'); \
    \
    (void)printf("%s: %d: %s(): ", (_s == NULL ? __FILE__ : (_s + 1)), \
        __LINE__, __FUNCTION__); \
    (void)printf x; (void)printf("\n"); \
} while (FALSE);


/**
 * @def MIN(x, y)
 * Evaluates to the minimum of x and y.
 */
/**
 * @def MAX(x, y)
 * Evaluates to the maximum of x and y.
 */
#if defined(MIN)
#warning "Redefining MIN in vcitypes.h"
#undef MIN
#endif  /* MIN */
#define MIN(x, y)                       (((x) < (y)) ? (x) : (y))

#if defined(MAX)
#warning "Redefining MAX in vcitypes.h"
#undef MAX
#endif  /* MAX */
#define MAX(x, y)                       (((x) > (y)) ? (x) : (y))


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef signed char int8;
typedef signed short int16;
typedef signed long int32;
typedef unsigned long long uint64;
typedef signed long long int64;
/*
#if (defined(__GNUC__) && !defined(__STRICT_ANSI__))
#if (__WORDSIZE == 64)
typedef unsigned long uint64;
typedef signed long int64;
#else
typedef unsigned long long uint64;
typedef signed long long int64;
#endif  
#endif  
*/

typedef signed char CHAR;
typedef signed short WCHAR;
typedef signed short SHORT;
typedef signed int LONG;
#if (defined(__GNUC__) && !defined(__STRICT_ANSI__))
#if (__WORDSIZE == 64)
typedef signed long LONGLONG;
#else
typedef signed long long LONGLONG;
#endif  /* __WORDSIZE == 64 */
#endif  /* (defined(__GNUC__) && !defined(__STRICT_ANSI__)) */

typedef unsigned char BYTE;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned int ULONG;
#if (defined(__GNUC__) && !defined(__STRICT_ANSI__))
#if (__WORDSIZE == 64)
typedef unsigned long ULONGLONG;
#else
typedef unsigned long long ULONGLONG;
#endif  /* __WORDSIZE == 64 */
#endif  /* (defined(__GNUC__) && !defined(__STRICT_ANSI__)) */

typedef float FLOAT;
typedef double DOUBLE;
/*@-isoreserved@*/
typedef unsigned int ERR_CODE;
/*@=isoreserved@*/
typedef int BOOLEAN;

typedef void *PVOID;
typedef char *STR;
typedef BYTE *PBYTE;


extern inline unsigned int hweight32(uint32 x)
{
    uint32 w;

    w = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    w = (w & 0x33333333) + ((w >> 2) & 0x33333333);
    w = (w & 0x0F0F0F0F) + ((w >> 4) & 0x0F0F0F0F);
    w = (w & 0x00FF00FF) + ((w >> 8) & 0x00FF00FF);
    w = (w & 0x0000FFFF) + ((w >> 16) & 0x0000FFFF);
    return((unsigned int)w);
}

extern inline unsigned int hweight16(uint16 x)
{
    uint16 w;

    w = (x & 0x5555) + ((x >> 1) & 0x5555);
    w = (w & 0x3333) + ((w >> 2) & 0x3333);
    w = (w & 0x0F0F) + ((w >> 4) & 0x0F0F);
    w = (w & 0x00FF) + ((w >> 8) & 0x00FF);
    return((unsigned int)w);
}

extern inline unsigned int hweight8(uint8 x)
{
    uint8 w;

    w = (x & 0x55) + ((x >> 1) & 0x55);
    w = (w & 0x33) + ((w >> 2) & 0x33);
    w = (w & 0x0F) + ((w >> 4) & 0x0F);
    return((unsigned int)w);
}
#endif

#ifdef __CYGWIN__
/*****************************************************************************
******************************************************************************
**                                                                          **
**  Compiler / Platform:                                                    **
**    CygWin                                                                **
**                                                                          **
**  Author:                                                                 **
**    Arthur Shipkowski                                                     **
**                                                                          **
**  Maintainer:                                                             **
**    Arthur Shipkowski                                                     **
**                                                                          **
**  Notes:                                                                  **
**    NONE                                                                  **
**                                                                          **
**  Revision History:                                                       **
**    03/14/02  AES   File origination                                      **
**                                                                          **
******************************************************************************
*****************************************************************************/

/*********************/
/* Value-based Types */
/*********************/
typedef signed char CHAR;                         /* 8  bits - signed character */
typedef signed short WCHAR;                       /* 16 bits - signed character (Unicode) */
typedef signed short SHORT;                       /* 16 bits - signed integer */
typedef signed int  LONG;                         /* 32 bits - signed integer */
#ifdef __alpha__
typedef signed long LONGLONG;                     /* 64 bits - signed integer */
#else
typedef signed long long LONGLONG;                /* 64 bits - signed integer */
#endif
typedef unsigned char BYTE;                       /* 8  bits - unsigned integer */
typedef unsigned char UCHAR;                      /* 8  bits - unsigned integer */
typedef unsigned short USHORT;                    /* 16 bits - unsigned integer */
typedef unsigned int  ULONG;                      /* 32 bits - unsigned integer */
#ifdef __alpha__
typedef unsigned long ULONGLONG;                  /* 64 bits - unsigned integer */
#else
typedef unsigned long long ULONGLONG;             /* 64 bits - unsigned integer */
#endif
typedef float FLOAT;                              /* 32 bits - floating point (real) */
typedef double DOUBLE;                            /* 64 bits - double precision */
typedef unsigned int ERR_CODE;                    /* 32 bits - unsigned integer */
typedef unsigned char BOOLEAN;                    /* 8  bits - boolean */

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned long       uint32;
typedef unsigned long long  uint64;
typedef signed long         int32;
typedef signed long long    int64;

/* Boolean values -- I think these are predefined, but it's best to be sure */
#ifndef TRUE                                      /* TRUE = 1 */
#define TRUE (1)
#endif

#ifndef FALSE                                     /* FALSE = 0 */
#define FALSE (0)
#endif

/*****************/
/* Pointer Types */
/*****************/
typedef void *PVOID;                              /* 32 bits - void pointer */
typedef BYTE *PBYTE;
typedef CHAR *STR;                                /* 8*(strlen+1) bits (Null Terminated)*/
#define MIN min
#define min(a,b) ((a)<(b)?(a):(b))

#define MAX(x, y)                       (((x) > (y)) ? (x) : (y))

#define size_t unsigned int

#endif


#if defined(DVD_PLATFORM_ONE)
/*****************************************************************************
******************************************************************************
**                                                                          **
**  Compiler / Platform:                                                    **
**    DVD Platform 1, using Nucleus.                                        **
**                                                                          **
**  Author:                                                                 **
**    Doug Kelly                                                            **
**                                                                          **
**  Maintainer:                                                             **
**    Doug Kelly                                                            **
**                                                                          **
**  Notes:                                                                  **
**    NONE                                                                  **
**                                                                          **
**  Revision History:                                                       **
**    06/27/03  DAK   File origination                                      **
**                                                                          **
******************************************************************************
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NUCLEUS
#include "nucleus.h"
#define NUCLEUS
#endif // ifndef nucleus

#include "img_defs.h"

#ifdef __cplusplus
}
#endif

/* Pointers */
typedef void *PVOID;                              /* 32 bits - void pointer */
typedef unsigned char *PBYTE;                     /* 8  bits - void byte pointer */

/* -- Unsigned Types -- */

typedef unsigned char  UCHAR;                     /* 8  bits - unsigned integer */
typedef unsigned short USHORT;                    /* 16 bits - unsigned integer */
typedef unsigned long long UINT64;                /* 64 bits - unsigned integer */

/* -- Signed Types -- */

typedef signed short   SHORT;                     /* 16 bits - signed integer */
typedef signed int     LONG;                      /* 32 bits - signed integer */
typedef signed long long INT64;                   /* 64 bits - signed integer */

/* Error Code */
typedef unsigned long  ERR_CODE;                  /* 32 bits - unsigned integer */



/* Boolean values */
typedef unsigned char  BOOLEAN;                   /* 8  bits - boolean */
#ifndef TRUE                                      /* TRUE = 1 */
#define TRUE (1)
#endif

#ifndef FALSE                                     /* FALSE = 0 */
#define FALSE (0)
#endif

#endif /* DVD_PLATFORM_ONE */

#endif /* VCITYPES_H */
