/*
 * sci.h
 *
 *  Created on: 2010/11/27
 *      Author: pino
 */

#ifndef _SCI_H_
#define _SCI_H_

typedef unsigned char u1;
typedef unsigned int short u2;

#define BIT_CTL
#define C20MHZ 1
#define C25MHZ 0

#ifndef BIT_CTL
#define CKS0	(1 << 0)
#define CKS1	(1 << 1)
#define MP		(1 << 2)
#define STOP	(1 << 3)
#define OE		(1 << 4)
#define PE		(1 << 5)
#define CHR	(1 << 6)
#define CA		(1 << 7)
#define SHIFT_CTL

#else

#define CKS_1C	0x00
#define CKS_4C	0x01
#define CKS_16C	0x02
#define CKS_64C	0x03
#endif

#if C20MHZ
typedef enum {	//内部クロック20MHz
  br4800  = 129,		// CKS=00
  br9600  = 64,		// CKS=00
  br19200 = 32,		// CKS=00
  br38400 = 15		// CKS=00
}BitRate;
#endif

#if C25MHZ
enum { 	//内部クロック25MHz
  br4800  = 162,		// CKS=00
  br9600  = 80,		// CKS=00
  br19200 = 40,		// CKS=00
  br38400 = 19		// CKS=00
}BitRate;
#endif

struct SCI
{
	union
	{
		u1 BYTE;
		struct
		{
			u1 CA		:1;		// 0=調歩同期 1=クロック同期
			u1 CHR		:1;		// 0=8bit data 1=7bit data
			u1 PE		:1;		// 0=パリティなし 1=パリティあり
			u1 OE		:1;		// 0=偶数パリティ 1=奇数パリティ
			u1 STOP	:1;		// 0=ストップビット長1bit 1=ストップビット長2bit
			u1 MP		:1;		// 0=シングルプロセッサ 1=マルチプロセッサ
			u1 CKS1	:1;		// クロックセレクト
			u1 CKS0	:1;		// クロックセレクト
		}BIT;

		struct
		{
			u1				:6;
			u1 CKS			:2;
		};
	}SMR;

	u1 BRR;

	union
	{
		u1 BYTE;
		struct
		{
			u1 TIE		:1;		// 送信完了時割り込み 0=TXI禁止 1=TXI許可
			u1 RIE		:1;		// 受信完了時割り込み 0=RXI禁止 1=RXI許可
			u1 TE		:1;		// 0=送信禁止 1=送信許可
			u1 RE		:1;		// 0=受信禁止 1=受信許可
			u1 MPIE	:1;		// マルチプロセッサ割り込み 0=禁止 1=許可
			u1 TEIE	:1;		// 送信終了割り込み 0=禁止 1=許可
			u1 CKE1	:1;		// 0=内部クロック(調歩同期) 1=外部クロック(クロック同期式)
			u1 CKE0	:1;		// CKE1=0,0=SCI は、SCK 端子を使用しません CKE1=1,0=ビットレートと同じ周波数のクロックを出力
		}BIT;
	}SCR;

	u1 TDR;

	union
	{
		u1 BYTE;
		struct
		{
			u1 TDRE	:1;		// 0=TDRに有効な送信データがライトされている 1=TDRに有効な送信データがない
								// ［クリア条件］TDRE=1 の状態をリードした後、0 をライトしたとき, DMAC でTDR へデータをライトしたとき
								// ［セット条件］TDR からTSR にデータ転送が行われてTDR にデータライトが可能になったとき, SCR のTE ビットが0 のとき, リセット、またはスタンバイモード時
			u1 RDRF	:1;		// 0=RDRに受信データが格納されていない 1=RDRに受信データが格納されている
								// ［クリア条件］DMACでRDR のデータをリードしたとき, RDRF=1 の状態をリードした後、0 をライトしたとき, リセット、またはスタンバイモード時
								// ［セット条件］シリアル受信が正常終了し、RSR からRDR へ受信データが転送されたとき
			u1 ORER	:1;		// 0=受信中、または正常に受信を完了した 1=受信時にオーバランエラーが発生した
			u1 FER_ERS	:1;		// 0=受信中、または正常に受信を完了した 1=受信時にフレーミングエラーが発生した
			u1 PER		:1;		// 0=受信中、または正常に受信を完了した 1=受信時にパリティエラーが発生した
			u1 TEND	:1;		// 0=送信中である 1=送信を終了した
			u1 MPB		:1;		// マルチプロセッサ関連
			u1 MPBT	:1;		//	"
		}BIT;
	}SSR;

	u1 RDR;

	union
	{
		u1 BYTE;
		struct
		{
			u1			:4;
			u1 SDIR	:1;
			u1 SINV	:1;
			u1			:1;
			u1 SMIF	:1;
		}BIT;
	}SCMR;
};

#define SCI0 0
#define SCI1 1
#define SCI2 2

#define SCI0A ((volatile struct SCI*)0xFFFFB0)
#define SCI1A ((volatile struct SCI*)0xFFFFB8)
#define SCI2A ((volatile struct SCI*)0xFFFFC0)

#define SMR0	0xFFFFB0 // シリアルモードレジスタ（SMR)
#define BRR0	0xFFFFB1 // ビットレートレジスタ（BRR）
#define SCR0	0xFFFFB2 // シリアルコントロールレジスタ（SCR）
#define TDR0	0xFFFFB3 // トランスミットデータレジスタ（TDR)		送信データ
#define SSR0	0xFFFFB4 // シリアルステータスレジスタ（SSR）
#define RDR0	0xFFFFB5 // レシーブシフトレジスタ（RSR）			受信データ
#define SCMR0	0xFFFFB6 // スマートカードモードレジスタ(SCMR)

#define SMR1	0xFFFFB8
#define BRR1	0xFFFFB9
#define SCR1	0xFFFFBa
#define TDR1	0xFFFFBb
#define SSR1	0xFFFFBc
#define RDR1	0xFFFFBd
#define SCMR1	0xFFFFBe

#define SMR2	0xFFFFC0
#define BRR2	0xFFFFC1
#define SCR2	0xFFFFC2
#define TDR2	0xFFFFC3
#define SSR2	0xFFFFC4
#define RDR2	0xFFFFC5
#define SCMR2	0xFFFFC6

void sci_init(int Sci_No, BitRate bitrate);
void sci_write(int Sci_No, char c);
void sci_write_str(int Sci_No,const char* _Str);
void sci_read(int Sci_No, char* buff, int size);
char sci_read_byte(int Sci_No);
int sci_read_pol(int Sci_No);

#endif /* SCI_H_ */
