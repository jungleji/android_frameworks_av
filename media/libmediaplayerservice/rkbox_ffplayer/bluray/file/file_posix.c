#if HAVE_CONFIG_H
#include "config.h"
#endif

//#if HAVE_MALLOC_H
#include <malloc.h>
//#endif

//#if HAVE_STDLIB_H
#include <stdlib.h>
//#endif

#include <stdio.h>

#include "include/file.h"
#include "include/macro.h"
#include "include/logging.h"

int file_mallocTime = 0;

FILE_H *file_open_linux(const char* filename, const char *mode);
void file_close_linux(FILE_H *file);
int64_t file_seek_linux(FILE_H *file, int64_t offset, int32_t origin);
int64_t file_tell_linux(FILE_H *file);
int file_eof_linux(FILE_H *file);
int file_read_linux(FILE_H *file, uint8_t *buf, int64_t size);
int file_write_linux(FILE_H *file, uint8_t *buf, int64_t size);

void file_close_linux(FILE_H *file)
{
    if (file) {
        fclose((FILE *)file->internal);

        DEBUG(DBG_FILE, "Closed LINUX file (%p)\n", file);
        file_mallocTime --;
        X_FREE(file);
    }
}

int64_t file_seek_linux(FILE_H *file, int64_t offset, int32_t origin)
{
    return fseeko((FILE *)file->internal, offset, origin);
}

int64_t file_tell_linux(FILE_H *file)
{
    return ftello((FILE *)file->internal);
}

int file_eof_linux(FILE_H *file)
{
    return feof((FILE *)file->internal);
}

int file_read_linux(FILE_H *file, uint8_t *buf, int64_t size)
{
    return fread(buf, 1, size, (FILE *)file->internal);
}

int file_write_linux(FILE_H *file, uint8_t *buf, int64_t size)
{
    return fwrite(buf, 1, size, (FILE *)file->internal);
}

FILE_H *file_open_linux(const char* filename, const char *mode)
{
    FILE *fp = NULL;
    FILE_H *file = malloc(sizeof(FILE_H));
    file_mallocTime ++;
 //   DEBUG(DBG_FILE, "Opening LINUX file %s... (%p)\n", filename, file);
    file->close = file_close_linux;
    file->seek = file_seek_linux;
    file->read = file_read_linux;
    file->write = file_write_linux;
    file->tell = file_tell_linux;
    file->eof = file_eof_linux;

    if ((fp = fopen(filename, mode))) {
        file->internal = fp;

        return file;
    }

    DEBUG(DBG_FILE, "Error opening file! (%p)\n", file);
    file_mallocTime --;
    X_FREE(file);

    return NULL;
}



