#ifndef MOOCOW_SAMPLETABLEBOX_H_INCLUDED
#define MOOCOW_SAMPLETABLEBOX_H_INCLUDED

#include "Box.h"

class SampleTableBox : public Box {
public:
	SampleTableBox(const void * p, size_t o) : Box(p, o) {
		make_container();
		children()->load(dataptr(), 0, data_length());
	}
};
#endif
