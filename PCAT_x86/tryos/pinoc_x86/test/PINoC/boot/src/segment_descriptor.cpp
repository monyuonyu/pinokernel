#include "segment_descriptor.h"

/* セグメントディスクリプタを生成
 31                                      1615                                      00
 ┌─────────────────────┬─────────────────────┐
                  base                               Limit
 ├────────┬─┬─┬─┬─┬────┼─┬──┬─┬───┬─┬────────┤
          base      G  DB  予 AVL  Limit     P  DPL  S   Type  A       Base
 └────────┴─┴─┴─┴─┴────┴─┴──┴─┴───┴─┴────────┘
 ※リトルエンディアンに注意
 */
vo segment_descriptor::io_sd16(SD* _sd, ui base, ui limit, u1 a, u1 type, u1 s, u1 dpl, u1 p, u1 avl,
		u1 db, u1 g)
{
	memset(_sd, 0x00, sizeof(_sd));

	_sd->g = g;

	_sd->db = db;

	_sd->avl = avl;

	// セグメント存在フラグ
	_sd->p = p;

	// 特権レベル
	_sd->dpl = dpl;

	// ディスクリプタ・タイプ
	_sd->s = s;

	// セグメント・タイプ
	_sd->type = type;

	// アクセス済みフラグ
	_sd->a = a;

}

vo segment_descriptor::LGDT16(u2 Limit, u4 Base)
{
	u1* _LGDT = (u1*)0x8400;	// 8400は気まぐれ

	// 8400番地にLimitのパラメータを格納
	*(u2*)_LGDT = Limit;

	_LGDT += 2;	// 2byteカウントアップ
	*(u4*)_LGDT = Base;

	// GDTRに格納
	__asm__(
			"LGDT 0x8400"
	);
}

vo segment_descriptor::LIDT16(u2 Limit, u4 Base)
{
	u1* _LGDT = (u1*)0x8400;	// 8400は気まぐれ

	// 8400番地にLimitのパラメータを格納
	*(u2*)_LGDT = Limit;

	_LGDT += 2;	// 2byteカウントアップ
	*(u4*)_LGDT = Base;

	// GDTRに格納
	__asm__(
			"LIDT 0x8400"
	);
}
