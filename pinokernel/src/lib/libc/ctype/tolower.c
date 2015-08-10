#include "kernel/ctype.h"
#include "kernel/libstr.h"
#include "kernel/basic.h"

int tolower(int c)
{
	if(c >= 'A' && c <= 'Z')
		return c + 0x20;

	return c;
}
