/*
 * XMODEM.c
 *
 *  Created on: 2011/02/27
 *      Author: mizu
 */

#include "3069f_sci.h"
#include "XMODEM.h"
#include "stdio.h"


#define ACK 0x06
#define NAK 0x15
#define EOT 0x04

void (*xwrite)(char c);
char (*xread)(void);
char (*xread_pol)(void);

#define XMODEM_DBG 0

// 通信が開始されると関数を抜けます
static int xmodem_start_nak()
{
	unsigned long cnt = 0;

	// 何か値が入るまで一定間隔ごとにNAKを送信
	while (!(xread_pol()))
	{
		if (cnt++ > 400000)
		{
			xwrite(XMODEM_NAK);
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
	block_num_rec = xread();
	if(block_num_now != block_num_rec)
		return 1;

	// ビット反転のブロック番号確認
	c = block_num_rec;
	block_num_rec = xread();
	block_num_rec ^= c;
	if(block_num_rec != 0xFF)
		return 1;

	// 128byteのデータ受信
	for(i = 0; i < XMODEM_BLOCK_SIZE; i++)
	{
		c = xread();
		*(buff++) = c;
		check_sum += c;
	}

	// チェックサム確認
	c = xread();
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
	int i;
//	int starting = 0;

	printf("-------- xmodem_start --------\n");

	xmodem_start_nak();

	while(1)
	{
		c = sci_read_byte(SCI_NO_1);

		if(c == XMODEM_SOH)
		{
			sci_read_byte(SCI_NO_1); //	ブロック
			sci_read_byte(SCI_NO_1); //	ブロック反転
			for(i = 0; i < 128; i++)
			{
				buf[i] = sci_read_byte(SCI_NO_1);
//				i++;
			}
			sci_read_byte(SCI_NO_1); // チェックサム
			sci_write(SCI_NO_1, XMODEM_ACK);
		}

		if(c == XMODEM_EOT)
		{
			sci_write(SCI_NO_1, XMODEM_ACK);
		}
	}








//	while(1)
//	{
//
////		printf("recv header\n");
//		c = xread(); // ヘッダ受信
//
//		switch(c)
//		{
//		case XMODEM_SOH:	// 通信開始！

//			if(xmodem_read_block(block_num_now, buf))
//			{
//				xwrite(XMODEM_NAK);	// 失敗を通知
//			}
//			else
//			{
//				block_num_now++;
//				buf += XMODEM_BLOCK_SIZE;		// バッファのポインタを1ブロック進める
//				xwrite(XMODEM_ACK);	// 成功を通知 → 次のブロック送信開始
//			}
//			break;
//
//		case XMODEM_EOT:	// 転送完了
////			printf("XMODEM_EOT\n");
//			xwrite(XMODEM_ACK);
//			return block_num_now;
//
//		case XMODEM_CAN:	// 中断
//			return 0;
//
//		case XMODEM_EOF:
//			return 0;
//
////		default:
//
//		}
//
//	}
}


void xmodem_init(void (*io_write)(char c), char (*io_read)(void), char (*io_read_pol)(void))
{
	xwrite = io_write;
	xread = io_read;
	xread_pol = io_read_pol;
}


