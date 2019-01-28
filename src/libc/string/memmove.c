#include <string.h>

void *memmove(void *dst, const void *src, size_t len)
    {
    register char *s1 = dst, *s2 = (char *) src;

    /* This bit of sneakyness c/o Glibc, it assumes the test is unsigned */
    if (s1 - s2 >= len)
        return memcpy(dst, src, len);

    /* This reverse copy only used if we absolutly have to */
    s1 += len;
    s2 += len;
    while (len-- > 0)
        *(--s1) = *(--s2);
    return dst;
    }

