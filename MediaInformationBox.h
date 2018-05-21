#ifndef MOOCOW_MEDIAINFORMATIONBOX_H_INCLUDED
#define MOOCOW_MEDIAINFORMATIONBOX_H_INCLUDED

#include "Box.h"

class MediaInformationBox : public Box {
public:
	MediaInformationBox(const void * p, size_t o) : Box(p, o) {
		make_container();
		children()->load(dataptr(), 0, data_length());
	}
};

#endif
