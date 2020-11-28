#ifndef __UTIL__
#define __UTIL__

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static inline void* ch8_malloc(size_t size)
{
    assert(size > 0);
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

static inline void ch8_free(void** pptr)
{
    if (*pptr != NULL) {
        free(*pptr);
        *pptr = NULL;
    }
}

#define ch8_max(a, b) (((a) > (b)) ? (a) : (b))
#define ch8_min(a, b) (((a) < (b)) ? (a) : (b))

#endif
