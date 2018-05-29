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

#include "Utf.h"

class Boxes;

class Box {
public:
	typedef std::function<Box *(const void *, size_t)> box_allocator_t;
private:
	const void * m_baseptr;
	size_t m_offset;
	typedef std::map<std::string, box_allocator_t> registry_map_t;
	static registry_map_t registry;
	std::shared_ptr<Boxes> subboxes;
protected:
	void make_container();
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
		if (x[0] < 0 || x[1] < 0 || x[2] < 0 || x[3] < 0) {
			return Utf::utf8(x, 4);
		}
		else {
			return std::string(x);
		}
	}
	static uint32_t fourcc(const char * x) {
		char bx[4] = { 0 };
		if (x[0] < 0 || x[1] < 0 || x[2] < 0 || x[3] < 0) {
			std::string y = Utf::latin1(x);
			strncpy(bx, y.c_str(), 4);
		}
		else {
			strncpy(bx, x, 4);
		}
		uint32_t y = 0;
		memmove(&y, bx, 4);
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
	virtual size_t data_length() const {
		return size() - header_length();
	}
	bool is_container() const {
		return (bool)subboxes;
	}
	const Boxes * children() const {
		return subboxes.get();
	}
	Boxes * children() {
		return subboxes.get();
	}
	virtual void json(Json::Value & obj) const;
	Json::Value json() const {
		Json::Value obj(Json::objectValue);
		json(obj);
		if (obj.size() == 2) {
			const size_t max_fmt_bytes = 128;
			const size_t fmt_buf_chars = max_fmt_bytes * 2 + 3 /* U+2026 in UTF-8 */ + 1;
			std::unique_ptr<char[]> fmt(new char[fmt_buf_chars]);
			memset(fmt.get(), 0, fmt_buf_chars);
			size_t s = data_length();
			bool binary = false;
			const uint8_t * p = reinterpret_cast<const uint8_t *>(dataptr());
			for (size_t i = 0; i < s && i < max_fmt_bytes; ++i) {
				if(p[i] < ' ' || p[i] > '~') {
					binary = true;
					break;
				}
			}
			char * q = fmt.get();
			if (binary) {
				for (size_t i = 0, j = 0; i < s && i < max_fmt_bytes; ++i, j += 2) {
					int hi = (p[i] >> 4) & 15, lo = (p[i] & 15);
					q[j] = static_cast<char>((hi > 9) ? (hi - 10 + 'a') : ('0' + hi));
					q[j + 1] = static_cast<char>((lo > 9) ? (lo - 10 + 'a') : ('0' + lo));
				}
			}
			else
			{
				memmove(q, p, std::min(s, max_fmt_bytes));
			}
			if (s > max_fmt_bytes) {
				strcat(q, "\xE2\x80\xA6");
			}
			obj["data"] = fmt.get();
		}
		return obj;
	}
};

class FullBox : public Box {
protected:
	FullBox(const void * p, size_t o) : Box(p, o) {}
public:
	virtual size_t header_length() const {
		return Box::header_length() + 4;
	}
	uint8_t version() const {
		uint32_t n = big32(*reinterpret_cast<const uint32_t *>(dataptr()));
		return (n >> 24) & 0xFF;
	}
	uint32_t flags() const {
		uint32_t n = big32(*reinterpret_cast<const uint32_t *>(dataptr()));
		return n & 0xFFFFFF;
	}
	void json(Json::Value &obj) const {
		Box::json(obj);
		obj["version"] = version();
		obj["flags"] = flags();
	}
};

class Boxes {
private:
	typedef std::shared_ptr<Box> boxptr_t;
	typedef std::vector<boxptr_t> list_t;
	list_t boxes;
public:
	typedef list_t::const_iterator const_iterator;
	typedef list_t::const_reverse_iterator const_reverse_iterator;
	Boxes() {}
	size_t load(const void * baseptr, size_t ofs, size_t len);
	Boxes(const void * baseptr, size_t ofs, size_t len) {
		load(baseptr, ofs, len);
	}
	size_t count() const {
		return boxes.size();
	}
	size_t count(uint32_t boxtype) const {
		size_t rv = 0;
		for (list_t::const_iterator it = boxes.begin(); it != boxes.end(); ++it) {
			if ((*it)->boxtype() == boxtype) {
				++rv;
			}
		}
		return rv;
	}
	size_t count(const char * boxtype) const {
		return count(Box::fourcc(boxtype));
	}
	const Box * at(size_t index) const {
		if (index < boxes.size())
		{
			return boxes[index].get();
		}
		else
		{
			return nullptr;
		}
	}
	const Box * of(uint32_t boxtype, size_t occurrence = 0) const {
		for (list_t::const_iterator it = boxes.begin(); it != boxes.end(); ++it) {
			if ((*it)->boxtype() == boxtype) {
				if (!occurrence)
				{
					return (*it).get();
				}
				else
				{
					--occurrence;
				}

			}
		}
		return nullptr;
	}
	const Box * of(const char * boxtype, size_t occurrence = 0) const {
		return of(Box::fourcc(boxtype), occurrence);
	}
	Boxes & append(const std::shared_ptr<Box> & box) {
		boxes.push_back(box);
		return *this;
	}
	const_iterator begin() const {
		return boxes.begin();
	}
	const_iterator end() const {
		return boxes.end();
	}
	const_reverse_iterator rbegin() const {
		return boxes.rbegin();
	}
	const_reverse_iterator rend() const {
		return boxes.rend();
	}
	const Box * operator[](size_t index) const {
		return at(index);
	}
	const Box * operator[](const char * boxtype) const {
		return of(boxtype);
	}
	void json(Json::Value & arr) const {
		for (const_iterator it = begin(); it != end(); ++it) {
			arr.append((*it)->json());
		}
	}
	Json::Value json() const {
		Json::Value arr(Json::arrayValue);
		json(arr);
		return arr;
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
