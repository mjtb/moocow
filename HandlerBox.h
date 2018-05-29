#ifndef MOOCOW_HANDLERBOX_H_INCLUDED
#define MOOCOW_HANDLERBOX_H_INCLUDED

#include "Box.h"

class HandlerBox : public FullBox {
public:
	HandlerBox(const void * p, size_t o) : FullBox(p, o) {}
	uint32_t handler_type() const {
		return big32(*reinterpret_cast<const uint32_t *>(dataptr(4)));
	}
	std::string name() const {
		size_t sz = data_length();
		if (sz > 20)
		{
			return std::string(reinterpret_cast<const char *>(dataptr(20)));
		}
		else
		{
			return std::string();
		}
	}
	void json(Json::Value & obj) const {
		FullBox::json(obj);
		obj["handler_type"] = fourcc(handler_type());
		obj["name"] = name();
	}
};
#endif
