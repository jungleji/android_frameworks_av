
#ifndef _STRUTL_H_
#define _STRUTL_H_

#include "attributes.h"
 
extern char * str_printf(const char *fmt, ...) BD_ATTR_FORMAT_PRINTF(1,2) BD_ATTR_MALLOC;
extern void str_tolower(char *s);
extern char * str_dup(const char *str);

#define MAKE_WORD(p) (((unsigned short)(((unsigned char*)p)[0]) << 8) | (unsigned short)((unsigned char*)p)[1])


#define MAKE_DWORD(p) (((unsigned int)((unsigned char*)p)[0] << 24) | ((unsigned int)((unsigned char *)p)[1] << 16) | \
                    ((unsigned int)((unsigned char *)p)[2] << 8) | (unsigned int)((unsigned char *)p)[3])

#define BD_MPEG2_TS_SIZE 192
#define MAX_PES_PACKAET 65535*2


#endif // STRUTL_H_
