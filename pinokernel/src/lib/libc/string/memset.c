#include "kernel/ctype.h"
#include "kernel/libstr.h"
#include "kernel/basic.h"

void *memset(void *s, int c, size_t n)
{
	register unsigned char* _s = (unsigned char *)s;
	register unsigned char _c = (unsigned char)c;

	int cnt;
	for(cnt = 0; cnt < n; cnt++)
	{
		*_s++ = _c;
	}

	return s;
}
