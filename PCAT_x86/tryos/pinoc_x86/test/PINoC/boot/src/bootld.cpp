#include "../../../common/typedef.h"
#include "../head/lowCall16.h"
#include "../head/segment_descriptor.h"
#include "../head/realmode.h"


vo waitkeybord();
vo io_A20GATE();
vo io_PROTECTMODE();

/*******************************************************************************
 		メイン関数
 *******************************************************************************/
vo main16()
{
	lowcall16 low16;



#if DEBUG
	low16.io_debug16();
#endif

	// VBEへ
	if(low16.vbe_mode())
		low16.vga_mode();


	// メモリの使用可能領域を2^32まで拡張
	io_A20GATE();

//	RGB color;
//	color.B = 0xFF;
//	color.G = 0xCC;
//	color.R = 0xCC;
//
//	RGB color2;
//	color2.B = 0x99;
//	color2.G = 0x99;
//	color2.R = 0x99;
//
//	RGB color3;
//	color3.B = 0x66;
//	color3.G = 0x66;
//	color3.R = 0x66;
//
//	// 1024x768	24bit/32bit
//	io_RGBwrite16(1024, 768, 0, 0, color);
//	io_RGBwrite16(1024, 1, 0, 768 - 27, color3);
//	io_RGBwrite16(1024, 26, 0, 768 - 26, color2);
//	io_RGBwrite16(64, 22, 2, 768 - 26 + 1, color3);

	// ジャンプ先にHLTLOOPを設定
	*(u1*)0x6500 = (u1)0xf4;
	*(u2*)0x6501 = (u2)0xfdeb;

	*(u1*)0x6000 = (u1)0xf4;
	*(u2*)0x6001 = (u2)0xfdeb;

	*(u8*)0x9008 = (u8)0x00cf9a006000FFFF;
	*(u8*)0x9010 = (u8)0x00cf9a006500FFFF;

	// GDTRとIDTRの設定をします。  -> 6byte
	segment_descriptor sd;
	sd.LGDT16(0x00FF, 0x00009000);
	sd.LIDT16(0x00FF, 0x00008500);


	/*--------------------------------------------------------------------------------
	 		パイプラインフラッシュの後、スタックを設定し、main32()へjmpします
	 --------------------------------------------------------------------------------*/
	io_PROTECTMODE();

}
#include "../head/32mode.hpp"


