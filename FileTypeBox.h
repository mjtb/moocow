#ifndef MOOCOW_FILETYPEBOX_H_INCLUDED
#define MOOCOW_FILETYPEBOX_H_INCLUDED

#include <vector>
#include "Box.h"

class FileTypeBox : public Box {
public:
	FileTypeBox(const void * p, size_t ofs) : Box(p, ofs) {}
	uint32_t major_brand() const {
		return big32(*reinterpret_cast<const uint32_t *>(dataptr(0)));
	}
	uint32_t minor_version() const {
		return big32(*reinterpret_cast<const uint32_t *>(dataptr(4)));
	}
	size_t num_compatible_brands() const {
		return (size() - (header_length() + 8)) / 4;
	}
	bool has_compatible_brands() const {
		return num_compatible_brands() > 0;
	}
	std::vector<uint32_t> compatible_brands() const {
		std::vector<uint32_t> rv;
		size_t n = num_compatible_brands();
		if (n > 0)
		{
			const uint32_t * p = reinterpret_cast<const uint32_t *>(dataptr(8));
			for (size_t i = 0; i < n; ++i)
			{
				rv.push_back(big32(p[i]));
			}
		}
		return rv;
	}
	virtual void json(Json::Value & obj) const {
		Box::json(obj);
		obj["major_brand"] = fourcc(major_brand());
		obj["minor_version"] = minor_version();
		Json::Value arr(Json::arrayValue);
		std::vector<uint32_t> vec = compatible_brands();
		for (std::vector<uint32_t>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
			arr.append(fourcc(*it));
		}
		obj["compatible_brands"] = arr;
	}
};

#endif
