#ifndef MOOCOW_METABOX_H_INCLUDED
#define MOOCOW_METABOX_H_INCLUDED

#include "Box.h"

class MetaBox : public Box {
public:
	MetaBox(const void * p, size_t o) : Box(p, o) {
		make_container();
		children()->load(dataptr(), 0, data_length());
	}
};

#endif

