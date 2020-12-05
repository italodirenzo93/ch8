#ifndef __UTIL_H__
#define __UTIL_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define STUBBED(x) printf("STUBBED: %s\n", x)

static inline void* ch8_malloc(size_t size)
{
    assert(size > 0);
    void *ptr = malloc(size);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }
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

#ifdef __cplusplus
}
#endif

#endif
