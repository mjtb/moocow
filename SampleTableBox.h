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

class BitRateBox : public Box {
public:
	BitRateBox(const void * p, const size_t o) : Box(p, o) {}
	uint32_t bufferSizeDB() const {
		return big32(*reinterpret_cast<const uint32_t *>(dataptr(0)));
	}
	uint32_t maxBitrate() const {
		return big32(*reinterpret_cast<const uint32_t *>(dataptr(4)));
	}
	uint32_t avgBitrate() const {
		return big32(*reinterpret_cast<const uint32_t *>(dataptr(8)));
	}
	void json(Json::Value & obj) const {
		Box::json(obj);
		obj["bufferSizeDB"] = bufferSizeDB();
		obj["maxBitrate"] = maxBitrate();
		obj["avgBitrate"] = avgBitrate();
	}
};

class SampleDescriptionBox : public FullBox {
public:
	SampleDescriptionBox(const void * p, size_t o) : FullBox(p, o) {
		make_container();
		children()->load(dataptr(4), 0, data_length() - 4);
	}
	uint32_t entry_count() const {
		return big32(*reinterpret_cast<const uint32_t *>(dataptr(0)));
	}
	void json(Json::Value & obj) const {
		FullBox::json(obj);
		obj["entry_count"] = entry_count();
	}
};

class TimeToSampleBox : public FullBox {
public:
	struct entry_t {
		uint32_t sample_count;
		uint32_t sample_delta;
	};
	typedef std::vector<entry_t> list_t;
	typedef list_t::const_iterator const_iterator;
private:
	list_t list;
public:
	TimeToSampleBox(const void *p, size_t o) : FullBox(p, o) {
		uint32_t ec = entry_count();
		for (uint32_t i = 0; i < ec; ++i) {
			entry_t e = { 0 };
			e.sample_count = big32(*reinterpret_cast<const uint32_t *>(dataptr(4 + i * 8)));
			e.sample_delta = big32(*reinterpret_cast<const uint32_t *>(dataptr(8 + i * 8)));
			list.push_back(e);
		}
	}
	uint32_t entry_count() const {
		return big32(*reinterpret_cast<const uint32_t *>(dataptr()));
	}
	const list_t & entries() const {
		return list;
	}
	list_t & entries() {
		return list;
	}
	const entry_t * operator[](size_t index) const {
		if (index < list.size()) {
			return &list[index];
		}
		else {
			return nullptr;
		}
	}
	void json(Json::Value & obj) const {
		FullBox::json(obj);
		uint32_t ec = entry_count();
		obj["entry_count"] = ec;
		Json::Value arr(Json::arrayValue);
		for (uint32_t i = 0; i < ec; ++i) {
			Json::Value v(Json::objectValue);
			v["sample_count"] = list[i].sample_count;
			v["sample_delta"] = list[i].sample_delta;
			arr.append(v);
		}
		obj["entries"] = arr;
	}
};
#endif
