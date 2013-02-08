/*
 * string.h
 *
 *  Created on: 2010/11/28
 *      Author: pino
 */

#ifndef _STRING_H_
#define _STRING_H_

#ifndef SIZE_T
#define SIZE_T
typedef unsigned int size_t;
#endif

void *memset(void *s, int c, size_t n);
void *memcpy(void * s1, const void * s2, size_t n);


#endif
