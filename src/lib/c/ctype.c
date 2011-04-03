/*
 * ctype.c
 *
 *  Created on: 2011/03/28
 *      Author: mizu
 */


#include "ctype.h"

int tolower(int c)
{
	if(c >= 'A' && c <= 'Z')
		return c + 0x20;

	return c;
}

int toupper(int c)
{
	if(c >= 'a' && c <= 'z')
		return c - 0x20;

	return c;
}
