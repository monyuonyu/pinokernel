#include "../../common/typedef.h"
#include "../../common/global.h"
#include "protectmode.h"

vo io_sd32(SD* _sd, ui base, ui limit, u1 a, u1 type, u1 s, u1 dpl, u1 p, u1 avl, u1 db, u1 g);
vo io_write32(si xsize, si ysize, si xSrc, si ySrc, s1 color);
vo io_hlt32();

//	簡易カーネル
vo main()
{

}

#include "lowCall32.h"