/*
 * xmodem.h
 *
 *  Created on: 2011/02/27
 *      Author: mizu
 */


#ifndef __XMODEM_H__
#define __XMODEM_H__

#define XMODEM_SOH 0x01		// í êMäJén
#define XMODEM_STX 0x02		//
#define XMODEM_EOT 0x04		// ì]ëóäÆóπ
#define XMODEM_ACK 0x06		// ÇÊÇÎÇµÇ≠Ç®äËÇ¢ÇµÇ‹Ç∑
#define XMODEM_NAK 0x15		// é∏îs
#define XMODEM_CAN 0x18		// íÜíf
#define XMODEM_EOF 0x1a		// Ctrl-z

#define XMODEM_BLOCK_SIZE 128

int xmodem_start(char* buf);
void xmodem_init(void (*io_write)(char c), char (*io_read)(void), char (*io_read_pol)(void));

#endif
