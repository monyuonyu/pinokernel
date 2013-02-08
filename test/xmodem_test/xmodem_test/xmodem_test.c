/*
 * XMODEM.c
 *
 *  Created on: 2011/02/27
 *      Author: mizu
 */

#include "3069f_sci.h"
#include "XMODEM.h"

void main(void);
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif

#define ACK 0x06
#define NAK 0x15
#define EOT 0x04

void (*write)(char c);
char (*read)(void);
char (*read_pol)(void);

#define XMODEM_DBG 0

// 通信が開始されると関数を抜けます
static int xmodem_start_ack()
{
	unsigned long cnt = 0;

	// 何か値が入るまで一定間隔ごとにNAKを送信
	while (!(read_pol()))
	{
		if (cnt++ > 200000)
		{
			write(XMODEM_NAK);
			cnt = 0;

		}
	}
	return 0;
}


// XMODEM 1ブロック受信
static int xmodem_read_block(unsigned char block_num_now, char* buff)
{
	unsigned char c, block_num_rec, check_sum = 0;
//	char c;
	int i;

	// ブロック番号確認
	block_num_rec = read();
	if(block_num_now != block_num_rec)
		return 1;

	// ビット反転のブロック番号確認
	c = block_num_rec;
	block_num_rec = read();
	block_num_rec ^= c;
	if(block_num_rec != 0xFF)
		return 1;

	// 128byteのデータ受信
	for(i = 0; i < XMODEM_BLOCK_SIZE; i++)
	{
		c = read();
		*(buff++) = c;
		check_sum += c;
	}

	// チェックサム確認
	c = read();
	check_sum ^= c;
	if(check_sum)
		return 1;

	// 正常終了
	return 0;

}

// 転送が完了すると転送したブロック数。 失敗・中断すると0を返す
int xmodem_start(char* buf)
{
	unsigned char block_num_now = 1;
	char c;
	int starting = 0;

	while(1)
	{
		if(!starting)
		{
			xmodem_start_ack();
		}

		c = read(); // ヘッダ受信

		switch(c)
		{
		case XMODEM_SOH:	// 通信開始！
			starting = 1;
			if(xmodem_read_block(block_num_now, buf))
			{
				write(XMODEM_NAK);	// 失敗を通知
			}
			else
			{
				block_num_now++;
				buf += XMODEM_BLOCK_SIZE;		// バッファのポインタを1ブロック進める
				write(XMODEM_ACK);	// 成功を通知 → 次のブロック送信開始
			}
			break;

		case XMODEM_EOT:	// 転送完了
			write(XMODEM_ACK);
			return block_num_now;

		case XMODEM_CAN:	// 中断
			return 0;

		case XMODEM_EOF:
			return 0;

//		default:

		}

	}
}


void xmodem_init(void (*io_write)(char c), char (*io_read)(void), char (*io_read_pol)(void))
{
	write = io_write;
	read = io_read;
	read_pol = io_read_pol;
}

/**
 * ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
 * ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
 */


void test_write(char c)
{
	sci_write(SCI_NO_1, c);
}

char test_read()
{
	char c;
	c = sci_read_byte(SCI_NO_1);
	return c;
}

char test_read_pol()
{
	char c = 0x00;
	c = sci_read_pol(SCI_NO_1);
	return c;
}

void main(void)
{
	static char buff[512] = {0,};
	int i;

	sci_init(SCI_NO_1, BitRate_type_br9600);
	xmodem_init(test_write, test_read, test_read_pol);
	xmodem_start(buff);

	for(i = 0; i < 200; i++)
	{
		sci_write(SCI_NO_1, buff[i]);
	}

	while(1);
}

#ifdef __cplusplus
void abort(void)
{

}
#endif
