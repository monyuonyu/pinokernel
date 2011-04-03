#ifndef __LOWCALL16_H__
#define __LOWCALL16_H__

#include "../../../common/typedef.h"
#include "../../../lib/string.h"

class lowcall16
{
private:
	struct
	{
		s1* vram;
		u4 xsize;
		u4 ysize;
		u1 color_bit;

	} graphicmode;
public:
	vo vga_mode();
	si vbe_mode();

	vo io_hlt16();
	vo io_fin16();
	vo io_debug16();
	vo io_RGBwrite16(si xsize, si ysize, si xSrc, si ySrc, RGB rgb);
};


#endif
