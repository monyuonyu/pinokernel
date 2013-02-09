/*
 * main.c
 *
 *  Created on: 2010/11/27
 *      Author: pino
 */

//#include "3069s.h"
#include "3069f_io.h"
#include "3069f_sci.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"
#include "interrupt.h"
#include "intr.h"

extern int buf_start;
extern int text_start;

int main()
{
	// シリアル通信初期化
	sci_init(SCI_NO_1, BitRate_type_br9600);
	sci_write(SCI_NO_1, 'A');

	while(1);
	return 0;
}

void init()
{
	extern int bss_start, bss_end, data_start, data_end, rodata_end;

	memset(&bss_start, 0, (long)&bss_end - (long)&bss_start);					// Bssセクション初期化
	memcpy(&data_start, &rodata_end, (long)&data_end - (long)&data_start);		// dataセクション初期化
	memset(&buf_start, 0, (long*)0x600);										// bufセクション初期化

	// 割り込みDISABLE
	INTERUUPT_DISABLE

	// ソフト割り込み初期化
	softvec_init();

	INTERUUPT_ENABLE

	main();
}
