#include "kernel/ctype.h"
#include "kernel/libstr.h"
#include "kernel/basic.h"

int toupper(int c)
{
	if(c >= 'a' && c <= 'z')
		return c - 0x20;

	return c;
}
