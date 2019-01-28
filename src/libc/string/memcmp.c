#include <rtos.h>

#include <sys/types.h>

int memcmp(const void *s1, const void *s2, size_t len)
{
        unsigned char *c1 = (unsigned char *)s1;
        unsigned char *c2 = (unsigned char *)s2;

        while (len--) {
                if (*c1 != *c2)
                        return *c1 - *c2;
                c1++;
                c2++;
        }
        return 0;
}

