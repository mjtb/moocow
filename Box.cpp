#include "stdafx.h"

#include "Box.h"

size_t Boxes::load(const void * baseptr, size_t ofs, size_t len) {
	size_t n = 0;
	for (size_t i = 0; i < len;) {
		if (((i + 8) > len) || !*reinterpret_cast<const uint32_t*>(reinterpret_cast<const uint8_t *>(baseptr) + ofs + i)) {
			break;
		}
		std::shared_ptr<Box> box(Box::createBox(baseptr, ofs + i));
		boxes.push_back(box);
		i += box->size();
		++n;
	}
	return n;
}

void Box::json(Json::Value & obj) const {
	obj["boxtype"] = fourcc();
	obj["size"] = size();
	if (is_container()) {
		obj["children"] = children()->json();
	}
}

void Box::make_container() {
	if (!subboxes) {
		subboxes.reset(new Boxes);
	}
}