#include "util.h"

#include <assert.h>
#include <string.h>

void *ch8_malloc(size_t size)
{
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void ch8_free(void **pptr)
{
    assert(pptr != NULL);

    void *ptr = *pptr;
    if (ptr != NULL)
    {
        free(ptr);
        ptr = NULL;
    }
}
