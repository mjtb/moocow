#ifndef MOOCOW_MEDIABOX_H_INCLUDED
#define MOOCOW_MEDIABOX_H_INCLUDED

#include "Box.h"

class MediaBox : public Box {
public:
	MediaBox(const void * p, size_t ofs) : Box(p, ofs) {
		make_container();
		children()->load(dataptr(), 0, data_length());
	}
};

#endif
