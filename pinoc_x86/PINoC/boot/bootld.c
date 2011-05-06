#include "../../common/typedef.h"
#include "../../common/global.h"

vo io_graphic16(u1 mode);
vo io_A20GATE();
vo io_PROTECTMODE();
vo io_RGBwrite16(si xsize, si ysize, si xSrc, si ySrc, RGB rgb);
vo LGDT16(u2 LImit, u4 Base);
vo LIDT16(u2 Limit, u4 Base);

#include "realmode.h"
vo main16()
{

#if DEBUG
	__asm__(
			"movb    $0x0e,   %ah  \n\t"
			"movb    $3   ,   %al  \n\t"
			"int     $0x10         \n\t"
	);
#endif

	// VBEへ
	io_graphic16(2);

	// メモリの使用可能領域を2^32まで拡張
	io_A20GATE();

	RGB color;
	color.B = 0xFF;
	color.G = 0xCC;
	color.R = 0xCC;

	RGB color2;
	color2.B = 0x99;
	color2.G = 0x99;
	color2.R = 0x99;

	RGB color3;
	color3.B = 0x66;
	color3.G = 0x66;
	color3.R = 0x66;

	// 1024x768	24bit/32bit
	io_RGBwrite16(1024, 768, 0, 0, color);
	io_RGBwrite16(1024, 1, 0, 768 - 27, color3);
	io_RGBwrite16(1024, 26, 0, 768 - 26, color2);
	io_RGBwrite16(64, 22, 2, 768 - 26 + 1, color3);

	// ジャンプ先にHLTLOOPを設定
	*(u1*)0x6500 = (u1)0xf4;
	*(u2*)0x6501 = (u2)0xfdeb;

	*(u1*)0x6000 = (u1)0xf4;
	*(u2*)0x6001 = (u2)0xfdeb;

	*(u8*)0x9008 = (u8)0x00cf9a006000FFFF;
	*(u8*)0x9010 = (u8)0x00cf9a006500FFFF;

	// GDTRとIDTRの設定をします。  -> 6byte
	LGDT16(0x00FF, 0x00009000);
	LIDT16(0x00FF, 0x00008500);

//	io_fin16();

	// パイプラインフラッシュの後、スタックを設定し、main32()へjmpします
	io_PROTECTMODE();

}
#include "lowCall16.h"
#include "biosCall16.h"
#include "32mode.h"

//#include "protectmode.h"
//
//__asm__(
//		"movw	$0x6000,	%ax		\n\t"
//		"movw	%ax,		%ds		\n\t"
//		"movw	%ax,		%es		\n\t"
//		"movw	%ax,		%fs		\n\t"
//		"movw	%ax,		%gs		\n\t"
//
//);
//
//#include "lowCall32.h"
//
//vo main32()
//{
//
//	__asm__("nop");
//	while (1)
//	{
//		__asm__("hlt");
////				io_hlt32();
//	}
//}