#if HAVE_CONFIG_H
#include "config.h"
#endif

//#if HAVE_MALLOC_H
#include <malloc.h>
//#endif

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <dirent.h>

#include "include/dir.h"
#include "include/macro.h"
#include "include/logging.h"
#include "errno.h"
#include<android/log.h>

#define LOG_TAG "Dir_Posix"
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)

int dir_openTime = 0;
DIR_H *dir_open_posix(const char* dirname);
void dir_close_posix(DIR_H *dir);
int dir_read_posix(DIR_H *dir, DIRENT *ent);

void dir_close_posix(DIR_H *dir)
{
    if (dir) {
        closedir((DIR *)dir->internal);
        dir_openTime -- ;
        DEBUG(DBG_DIR, "Closed POSIX dir (%p)\n", dir);

        X_FREE(dir);
    }
}

int dir_read_posix(DIR_H *dir, DIRENT *entry)
{
    struct dirent e, *p_e;
    int result;

    result = readdir_r((DIR*)dir->internal, &e, &p_e);
    if (result) {
        return -result;
    } else if (p_e == NULL) {
        return 1;
    }
    strncpy(entry->d_name, e.d_name, 256);
    return 0;
}

DIR_H *dir_open_posix(const char* dirname)
{
    DIR *dp = NULL;
    DIR_H *dir = malloc(sizeof(DIR_H));
    dir_openTime ++;
    LOGD("Opening POSIX dir %s\n", dirname);
    dir->close = dir_close_posix;
    dir->read = dir_read_posix;

    if ((dp = opendir(dirname))) {
        dir->internal = dp;
        return dir;
    }
    LOGD("************strerror: %s\n", strerror(errno)); 
    LOGD("Error opening dir! (%p)\n", dir);
    dir_openTime --;
    X_FREE(dir);

    return NULL;
}
