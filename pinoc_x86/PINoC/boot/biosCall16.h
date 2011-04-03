#ifndef _BIOSCALL16_H_
#define _BIOSCALL16_H_

/* io_hlt16 */
__asm__(
		"io_hlt16:		\n\t"
		"hlt			\n\t"
		"ret			\n\t"
);

/* io_fin16 */
__asm__(
		"io_fin16:		\n\t"
		"hlt			\n\t"
		"jmp io_fin16	\n\t"
);

vo io_debug16()
{

	__asm__(
			"pushw	%ax				\n\t"
			"movb	$0x0e,	%ah		\n\t"
			"movb	$1,		%al		\n\t"
			"int	$0x10			\n\t"
			"popw	%ax				\n\t"
	);
}

vo io_load16()
{
	__asm__(
			"movw	$0x500,	%di		\n\t"
			"movw	%di,		%es		\n\t"
			"movw    $0x00,     %bx  \n\t"

			"movb    $0x00,     %ch  \n\t"
			"movb    $0x00,     %dh  \n\t"
			"movb    $0x02,     %cl  \n\t"

			"movw    $0x00,     %si  \n\t" // 失敗カウンタ初期化

			"io_load_retry:          \n\t"

			"movb    $0x02,     %ah  \n\t"
			"movb    $0x08,     %al  \n\t" // 連続したセクタr
			"movb    $0x00,     %dl  \n\t"
			"int     $0x13           \n\t" //	セクタ読み込み[bios]

			"jnc     io_load_success	\n\t"

			"add     $0x01,     %si  \n\t" //	失敗回数をカウントアップ
			"cmp     $0x05,     %si  \n\t" // 5回失敗したらエラー終了
			"jae     io_load_error   \n\t"

			"movb    $0x00,     %ah  \n\t"
			"movb    $0x00,     %dl  \n\t"
			"int     $0x13           \n\t"
			"jmp     io_load_retry   \n\t" //	初期化してリトライ
	);

	__asm__("io_load_error:");
#if DEBUG
	io_debug16();
#endif

	__asm__("io_load_success:");
#if DEBUG
	io_debug16();
#endif

}

#endif
