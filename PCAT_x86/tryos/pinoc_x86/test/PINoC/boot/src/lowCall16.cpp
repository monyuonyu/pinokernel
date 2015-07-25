#include "lowCall16.h"


vo lowcall16::vga_mode()
{
	__asm__(
			"movb    $0x00,    %ah	\n\t"
			"movb    $0x13,    %al	\n\t"
			"int     $0x10			\n\t"
	);
	graphicmode.vram = (s1*) 0xa0000;
	graphicmode.xsize = 320;
	graphicmode.ysize = 200;
	graphicmode.color_bit = 0x08;
}

si lowcall16::vbe_mode()
{
	__asm__(
			"push	%es					\n\t"
			"push	%di					\n\t"

			// 情報格納先アドレスセット VESA情報
			"movw	$0x800,	%ax		\n\t"
			"movw	%ax,		%es		\n\t"
			"movw	$0x00,		%di		\n\t"

			// VBE情報獲得
			"movw	$0x4f00,	%ax		\n\t"
			"int	$0x10				\n\t"
			"cmpw	$0x004f,	%ax		\n\t" // ah == 0 -> 正常終了 al == 4f->VESAサポート
			"jnz	no_support			\n\t" // VGAモードを設定
	);

	__asm__(
			// 情報格納先アドレスセット VBE情報
			"movw	$0x810,	%ax		\n\t"
			"movw	%ax,		%es		\n\t"
			"movw	$0x00,		%di		\n\t"

			// 画面モード情報獲得
			"movw	$0x4f01,	%ax		\n\t"
			"movw	$0x0118,	%cx		\n\t" // 1024x768	24bit/32bit
			"int	$0x10				\n\t"
			"cmpw	$0x004f,	%ax		\n\t" // ah == 0 -> 正常終了 al == 4f->VESAサポート
			"jnz	no_support		\n\t" // VGAモードを設定
	);

	// ビットカラー情報確保 1byte
	u1* src1 = (u1*) 0x8119;
	graphicmode.color_bit = *src1;

	// vram開始番地格納 4byte
	u4* src2 = (u4*)0x8128;

	graphicmode.vram = (s1*) *src2; // メモリの4byteの値を1byteのポインタへキャスト

	// 解像度 4byte
	graphicmode.xsize = 1024;
	graphicmode.ysize = 768;

	__asm__(
			// 画面モードを設定
			"movw	$0x4f02,	%ax		\n\t"
			"movw	$0x0118,	%bx		\n\t"
			"orw	$0x4000,	%bx		\n\t"
			"int	$0x10				\n\t"
			"cmpw	$0x004f,	%ax		\n\t" // ah == 0 -> 正常終了 al == 4f->VESAサポート
			"jnz	no_support			\n\t" // VGAモードを設定

			"pop	%di					\n\t"
			"pop	%es					\n\t"

			"movl	$0xE0000000,	%eax	\n\t"
			"movl	$0xFFFFFF00,	(%eax)	\n\t"
			"jmp	support				\n\t"
	);

	__asm__(":no_support		\n\t");
	return 1;

	__asm__(":support			\n\t");
	return 0;
}

/* io_hlt16 */
vo lowcall16::io_hlt16()
{
	__asm__(
			"io_hlt16:		\n\t"
			"hlt			\n\t"
			"ret			\n\t"
	);
}

/* io_fin16 */
vo lowcall16::io_fin16()
{
	__asm__(
			"io_fin16:		\n\t"
			"hlt			\n\t"
			"jmp io_fin16	\n\t"
	);
}

vo lowcall16::io_debug16()
{
	__asm__(
			"pushw	%ax				\n\t"
			"movb	$0x0e,	%ah		\n\t"
			"movb	$1,		%al		\n\t"
			"int	$0x10			\n\t"
			"popw	%ax				\n\t"
	);
}


vo lowcall16::io_RGBwrite16(si xsize, si ysize, si xSrc, si ySrc, RGB rgb)
{
	si si_i, si_ii;

	RGB* dst = (RGB*)graphicmode.vram;

	dst += graphicmode.xsize * ySrc;
	dst += xSrc;

	switch(graphicmode.color_bit)
	{
	case 24:
		FOR(si_i, ysize)
		{
			FOR(si_ii, xsize)
			{
				__asm__("nop");
				dst[si_ii] = rgb;
			}
			dst += graphicmode.xsize;
		}
		break;
	case 32:
		FOR(si_i, ysize)
		{
			FOR(si_ii, xsize)
			{
//				(u1*)dst += 4;
				__asm__("nop");
				dst[si_ii] = rgb;
			}
			dst += graphicmode.xsize;
		}
		break;

	}
}
