/*
 * ascii.cpp
 *
 *  Created on: 2011/05/03
 *      Author: mizu
 */
#include "kernel/ctype.h"
#include "kernel/libstr.h"
#include "kernel/basic.h"

// ASCIIコードをバイナリに変換
void asciitobin(char* _str, size_t size)
{
	char tmp[3] = {0,};
	int i;
	for(i = 0; i < size; i += 2)
	{
		tmp[0] = _str[i];
		tmp[1] = _str[i + 1];

		_str[i / 2] = (char)decto10(tmp);
	}

	memset(&_str[size / 2], 0x00, size / 2);
}
