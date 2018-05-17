#ifndef MOOCOW_BOX_H_INCLUDED

#define MOOCOW_BOX_H_INCLUDED


#include <functional>
#include <algorithm>
#include <string>
#include <map>

#ifdef _MSC_VER
#include <rpcdce.h>
#else
#include <linux/uuid.h>
typedef uuid_le uuid_t;
#endif

#include "json.h"

class Box {
public:
	typedef std::function<Box *(const void *, size_t)> box_allocator_t;
private:
	const void * m_baseptr;
	size_t m_offset;
	typedef std::map<std::string, box_allocator_t> registry_map_t;
	static registry_map_t registry;
protected:
	Box(const void * baseptr, size_t offset) : m_baseptr(baseptr), m_offset(offset) {}
public:
#define BIG16(x) be16toh(x)
#define BIG32(x) be32toh(x)
#define BIG64(x) be64toh(x)
#define REBIG16(x) htobe16(x)
#define REBIG32(x) htobe32(x)
#define REBIG64(x) htobe64(x)
	static inline uint16_t big16(uint16_t x) {
#ifdef _MSC_VER
		return _byteswap_ushort(x);
#else
		return be16toh(x);
#endif
	}
	static inline uint32_t big32(uint32_t x) {
#ifdef _MSC_VER
		return _byteswap_ulong(x);
#else
		return be32toh(x);
#endif
	}
	static inline uint64_t big64(uint64_t x) {
#ifdef _MSC_VER
		return _byteswap_uint64(x);
#else
		return be64toh(x);
#endif
	}
	static inline uint16_t rebig16(uint16_t x) {
#ifdef _MSC_VER
		return _byteswap_ushort(x);
#else
		return htobe16(x);
#endif
	}
	static inline uint32_t rebig32(uint32_t x) {
#ifdef _MSC_VER
		return _byteswap_ulong(x);
#else
		return htobe32(x);
#endif
	}
	static inline uint64_t rebig64(uint64_t x) {
#ifdef _MSC_VER
		return _byteswap_uint64(x);
#else
		return htobe64(x);
#endif
	}
	static void registerBoxType(const char * type, const box_allocator_t & allocator) {
		registry[std::string(type)] = allocator;
	}
	static std::string fourcc(uint32_t f) {
		uint32_t bef = rebig32(f);
		char x[5] = { 0 };
		memmove(x, &bef, 4);
		return std::string(x);
	}
	static uint32_t fourcc(const char * x) {
		uint32_t y = 0;
		memmove(&y, x, 4);
		return big32(y);
	}
	static uint32_t fourcc(const std::string & x) {
		return fourcc(x.c_str());
	}
	static Box * createBox(const void * baseptr, size_t offset) {
		std::string fcc = fourcc(big32(*reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(baseptr) + offset + 4)));
		registry_map_t::const_iterator it = registry.find(fcc);
		if (it != registry.end()) {
			return it->second(baseptr, offset);
		}
		else {
			return new Box(baseptr, offset);
		}
	}
	virtual ~Box() {}
	const void * ptr(size_t relofs = 0) const {
		return reinterpret_cast<const uint8_t *>(m_baseptr) + m_offset + relofs;
	}
	const void * dataptr(size_t relofs = 0) const {
		return ptr(header_length() + relofs);
	}
	bool has_largesize() const {
		uint32_t x = big32(*reinterpret_cast<const uint32_t *>(ptr(0)));
		return x == 1;
	}
	size_t size() const {
		uint32_t x = big32(*reinterpret_cast<const uint32_t *>(ptr(0)));
		if (x == 1)
		{
			uint64_t y = big64(*reinterpret_cast<const uint64_t *>(ptr(8)));
			return static_cast<size_t>(std::min<uint64_t>(std::numeric_limits<size_t>::max(), y));
		}
		else
		{
			return x;
		}
	}
	uint64_t largesize() const {
		uint32_t x = big32(*reinterpret_cast<const uint32_t *>(ptr(0)));
		if (x == 1)
		{
			return big64(*reinterpret_cast<const uint64_t *>(ptr(8)));
		}
		else
		{
			return x;
		}
	}
	uint32_t boxtype() const {
		return big32(*reinterpret_cast<const uint32_t *>(ptr(4)));
	}
	bool has_usertype() const {
		return boxtype() == 0x75756964; // 'uuid'
	}
	uuid_t usertype() const {
		uuid_t rv = { 0 };
		if (has_usertype())
		{
			memmove(&rv, ptr(has_largesize() ? 16 : 8), 16);
		}
		return rv;
	}
	std::string fourcc() const {
		return fourcc(boxtype());
	}
	virtual size_t header_length() const {
		return 8 + (has_largesize() ? 8 : 0) + (has_usertype() ? 16 : 0);
	}
	virtual void json(Json::Value & obj) const {
		obj["boxtype"] = fourcc();
		obj["size"] = size();
	}
	Json::Value json() const {
		Json::Value obj(Json::objectValue);
		json(obj);
		return obj;
	}
};

template<class T>
class BoxTypeRegistration
{
public:
	BoxTypeRegistration(const char * fourcc) {
		Box::registerBoxType(fourcc, [](const void * p, size_t o) -> Box * { return new T(p, o); });
	}
};

#endif
