/*
 * DP8390.h
 *
 *  Created on: 2010/12/06
 *      Author: mizu
 */

#ifndef _DP8390_H_
#define _DP8390_H_

unsigned char u1;

#define page0

// DP8390制御レジスタ
struct DP8390
{
	// 00h
	union // コマンドレジスタ PS=00 PS=01 PS=10
	{
		struct
		{
			u1 STP :1; // すべてストップ
			u1 STA :1; // 開始
			u1 TXP :1; // パケット送信開始
			u1 RD0 :1; //
			u1 RD1 :1; // 000=禁止 001=リード 010=ライト 011パケット送信 1xx=強制停止
			u1 RD2 :1; // リモートDMA
			u1 PS0 :1; // 00=p0 01=p1 10=p2 11=p予約
			u1 PS1 :1; // ページ選択
		};

		struct
		{
			u1 :6;
			u1 PS :2;
		};

		struct
		{

			u1 :3;
			u1 RD :3;
			u1 :2;
		};
	}CR;

	// 01h
	union
	{
		u1 CLDA0;	// PS=00(r) PS=10(w)
		u1 PSTART;	// PS=00(w) PS=10(r)
		u1 PAR0;	// PS=01
	};

	//02h
	union
	{
		u1 CLDA1;	// PS=00(r) PS=10(w)
		u1 PSTOP;	// PS=00(w) PS=10(r)
		u1 PAR1;	// PS=01
	};

	// 03h
	union
	{
		u1 BNRY;	// PS=00
		u1 PAR2;	// PS=01
		u1 RNPP;	// PS=10
	};

	//04h
	union
	{
		union // 送信ステータス・レジスタ PS=00
		{
			struct
			{
				u1 PTX :1;
				u1 :1; // 予約
				u1 COL :1;
				u1 ABT :1;
				u1 CSR :1;
				u1 FU :1;
				u1 CHD :1;
				u1 OWC :1;
			};
		}TSR;

		u1 TPSR;	// PS=00→(w) PS=10→(r)
		u1 PAR3;	// PS=01(rw)

	};

	// 05h
	union
	{
		u1 NCR;		// PS=00(r)
		u1 TBCR0;	// PS=00(w)
		u1 PAR4;	// PS=01(rw)
		u1 LNPP;	// PS=10(rw)
	};

	// 06h
	union
	{
		u1 FIFO;	// PS=00(r)
		u1 TBCR1;	// PS=00(w)
		u1 PAR5;	// PS=01(rw)
		u1 HAC;		// PS=10(r)
	};

	// 07h
	union // 割り込みステータス・レジスタ PS=00(rw)
	{
		struct
		{
			u1 RST :1;
			u1 RDC :1;
			u1 CNT :1;
			u1 OVW :1;
			u1 TXE :1;
			u1 RXE :1;
			u1 PTX :1;
			u1 PRX :1;
		};

		u1 CURR;	// PS=01(rw)
		u1 LAC;		// PS=10(rw)
	}ISR;

	// 08h
	union
	{
		u1 CRDA0;	// PS=00(r)
		u1 RSAD0;	// PS=00(w)
		u1 MAR0;	// PS=01(rw)
	};

	// 09h
	union
	{
		u1 CRSA1;	// PS=00(r)
		u1 RSAD1;	// PS=00(w)
		u1 MAR1;	// PS=01(rw)
	};

	// 0Ah
	union
	{
		u1 RBCR0;	// PS=00(w)
		u1 MAR2;	// PS=01(rw)
	};

	// 0Bh;
	union
	{
		u1 RBCR1;	// PS=00(w)
		u1 MAR3;	// PS=01(rw)
	};

	// 0Ch
	union
	{
		union // 受信ステータス・レジスタ PS=00(r)
		{
			struct
			{
				u1 PRX :1;
				u1 CRC :1;
				u1 FAE :1;
				u1 FO :1;
				u1 MPA :1;
				u1 PHY :1;
				u1 DIS :1;
				u1 DFR :1;
			};
		}RSR;

		// 受信コンフィギュレーション・レジスタ
		union  	// PS=00(w) PS=10(r)
		{
			struct
			{
				u1 SEP :1;
				u1 AR :1;
				u1 AB :1;
				u1 AM :1;
				u1 PRO :1;
				u1 MON :1;
				u1 :2; // 予約
			};
		}RCR;

		u1 MAR4;	// PS=01(rw)
	};

	// 0Dh
	union
	{
		u1 CNTR0;	// PS=00(r)

		 // 送信コンフィギュレーション・レジスタ
		union	// PS=00(w) PS=10(r)
		{
			struct
			{
				u1 CRC :1; // CRC制御
				u1 LB0 :1; // ループバック指定
				u1 LB1 :1; //
				u1 ATD :1; //
				u1 OFST :1; // 衝突オフセット
				u1 :3; // 予約

			};

			struct
			{
				u1 :1;
				u1 LB :2;
				u1 :5;
			};
		}TCR;

		u1 MAR5;	// PS=01(rw)
	};

	// 0Eh
	union
	{
		u1 CNTR1;	// PS=00(r)

		// データコンフィギュレーション・レジスタ
		union // PS=00(w) PS=10(r)
		{
			struct
			{
				u1 WTS :1; // ワード転送指定
				u1 BOS :1; // バイト順番指定
				u1 LAS :1; // ロングアドレス指定
				u1 LS :1; // ループバック指定
				u1 AR :1; // 自動初期化リモート
				u1 FT0 :1; // DMA転送を起動するタイミングの指定
				u1 FT1 :1; // FIFO スレッショルド指定
				u1 :1; // 予約
			}DCR;

			struct
			{
				u1 :5;
				u1 FT :2;
				u1 :1;
			};
		};

		u1 MAR6;	//PS=01(rw)
	};

	// 0Fh
	union
	{
		u1 CNTR2;	// PS=00(r)

		// 割り込みマスク・レジスタ PS=00(w) PS=10(r)
		union
		{
			struct
			{
				u1 PRXE :1;
				u1 PTXE :1;
				u1 RXEE :1;
				u1 TXEE :1;
				u1 OVWE :1;
				u1 CNTE :1;
				u1 RDCE :1;
				u1 :1;
			};
		}IMR;

		u1 MAR7;	// PS=01(rw)
	};
};

#endif /* DP8390_H_ */

