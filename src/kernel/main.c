/*
 * main.c
 *
 *  Created on: 2011/03/29
 *      Author: mizu
 */

#include "3069f_io.h"
#include "3069f_sci.h"
#include "dbg.hpp"


int main()
{
		char str[] = "Hello World!!\n\r";

		// ポート4を光らすテスト
		volatile unsigned char* p4ddr = (volatile unsigned char*) 0xFEE003;
		volatile unsigned char* p4 = (volatile unsigned char*) 0xFFFFD3;
		*p4ddr = 0x03;
		*p4 = 0x01;

		sci_write_str(SCI1, str);

		dbg();

		while(1);
	return 0;
}
