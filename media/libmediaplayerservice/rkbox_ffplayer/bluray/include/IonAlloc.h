#ifndef __IONALLOC_H__
#define __IONALLOC_H__

enum {
	FD_INIT = -1,
};

struct alloc_data {
    void           *base;
    int            fd;
    int            offset;
    size_t         size;
    size_t         align;
    unsigned int   pHandle;
    bool           uncached;
    unsigned int   flags;
    int            allocType;

    alloc_data()
    {
        base = NULL;
        fd = -1;
        offset = 0;
        size = 0;
        align = 1;
        pHandle = 0;
        uncached = false;
        flags = 0;
        allocType = 0;
    }
};

class IonAlloc {
public:
    int alloc_buffer(struct alloc_data* data);

    int free_buffer(void *base, size_t size,
            int offset, int fd);

    int map_buffer(void **pBase, size_t size,
            int offset, int fd);

    int unmap_buffer(void *base, size_t size,
            int offset, int fd);

    int clean_buffer(void*base, size_t size,
            int offset, int fd);

	IonAlloc() { mIonFd = FD_INIT; }

    ~IonAlloc() { close_device(); }
		
private:
    int mIonFd;
	int map_count;
    
    int open_device();
    void close_device();
};

#endif //__IONALLOC_H__