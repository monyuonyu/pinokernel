#ifndef SEGMENT_DESCRIPTOR_H_
#define SEGMENT_DESCRIPTOR_H_

#include "../../../common/typedef.h"
#include "../../../lib/string.h"

class segment_descriptor
{
private:
public:
	vo io_sd16(SD* _sd, ui base, ui limit, u1 a, u1 type, u1 s, u1 dpl, u1 p, u1 avl, u1 db, u1 g);
	vo LGDT16(u2 Limit, u4 Base);
	vo LIDT16(u2 Limit, u4 Base);
};


#endif /* SEGMENT_DESCRIPTOR_H_ */
