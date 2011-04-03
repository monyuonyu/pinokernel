/*
 * sci.c
 *
 *  Created on: 2010/11/27
 *      Author: pino
 */

#include "3069f_sci.h"

static struct{
	volatile struct SCI *sci;
}regs[3] =
{
	{SCI0A},
	{SCI1A},
	{SCI2A},
};

void sci_init(int Sci_No, BitRate b)
{
	volatile struct SCI* sci = regs[Sci_No].sci;

	sci->SCR.BYTE = 0;
	sci->SMR.BYTE = 0;
	sci->BRR = b;

	// 送受信許可
	sci->SCR.BIT.RE = 1;
	sci->SCR.BIT.TE = 1;

	sci->SSR.BYTE = 0;
}

void sci_write(int Sci_No, char c)
{
	volatile struct SCI* sci = regs[Sci_No].sci;

	while(!sci->SSR.BIT.TDRE);
	sci->TDR = c;
	sci->SSR.BIT.TDRE = 0;

//	while(sci->SSR.BIT.TEND);

}

void sci_write_str(int Sci_No,const char* _Str)
{
	volatile struct SCI* sci = regs[Sci_No].sci;

	while(*_Str)
	{
		sci_write(Sci_No, *_Str++);
	}

	// バッファの中身が全て送信されるまで待機
	while(!sci->SSR.BIT.TEND);
}


// データを1byteだけ受信する
char sci_read_byte(int Sci_No)
{
	volatile struct SCI* sci = regs[Sci_No].sci;
	char c;

	// データが格納されるまで待機
	while (1)
	{
		if (sci->SSR.BIT.ORER)
		{
			sci_write_str(Sci_No, "Error_sci over run");
			sci->SSR.BIT.ORER = 0;
		}
		if (sci->SSR.BIT.PER)
		{
			sci_write_str(Sci_No, "Error_sci parity");
			sci->SSR.BIT.PER = 0;
		}
		if (sci->SSR.BIT.FER_ERS)
		{
			sci_write_str(Sci_No, "Error_sci flaming");
			sci->SSR.BIT.FER_ERS = 0;
		}

		// データが格納されているか確認
		if (sci->SSR.BIT.RDRF)
			break;
	}

	c = sci->RDR;
	sci->SSR.BIT.RDRF = 0;

	return c;
}

// 連続したデータを受信
void sci_read(int Sci_No, char* buff, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		*(buff++) = sci_read_byte(Sci_No);
	}
}

// データを受信しているか確認する。
// ポーリングで確認する
// 格納されていれば1を返し、格納されていなければ、0を返す
int sci_read_pol(int Sci_No)
{
	volatile struct SCI* sci = regs[Sci_No].sci;

	if (sci->SSR.BIT.ORER)
	{
		sci_write_str(Sci_No, "Error_sci over run");
		sci->SSR.BIT.ORER = 0;
	}
	if (sci->SSR.BIT.PER)
	{
		sci_write_str(Sci_No, "Error_sci parity");
		sci->SSR.BIT.PER = 0;
	}
	if (sci->SSR.BIT.FER_ERS)
	{
		sci_write_str(Sci_No, "Error_sci flaming");
		sci->SSR.BIT.FER_ERS = 0;
	}

	// データが格納されているか確認
	if (sci->SSR.BIT.RDRF)
		return 1;

	return 0;
}

