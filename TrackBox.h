#ifndef MOOCOW_TRACKBOX_H_INCLUDED
#define MOOCOW_TRACKBOX_H_INCLUDED

#include "Box.h"

class TrackBox : public Box {
public:
	TrackBox(const void * p, size_t ofs) : Box(p, ofs) {
		make_container();
		children()->load(dataptr(), 0, data_length());
	}
};

#endif
