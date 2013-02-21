/*
 * string.c
 *
 *  Created on: 2010/11/28
 *      Author: pino
 */

#include "string.h"


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

void *memcpy(void * s1, const void * s2, size_t n)
{
	register unsigned char* _s1 = (unsigned char *)s1;
	register unsigned char* _s2 = (unsigned char *)s2;

	int cnt;
	for(cnt = 0; cnt < n; cnt++)
	{
		*_s1++ = *_s2++;
	}

	return s1;
}
