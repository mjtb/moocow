#ifndef MOOCOW_USERDATABOX_H_INCLUDED
#define MOOCOW_USERDATABOX_H_INCLUDED

#include "Box.h"
#include "Utf.h"

#include <cstdarg>

class UserDataBox : public Box {
public:
	UserDataBox(const void * p, size_t ofs) : Box(p, ofs) {
		make_container();
		children()->load(dataptr(), 0, size() - header_length());
	}
};

class UserDataStringList {
public:
	class string_t {
	public:
		std::string lang;
		std::string text;
		string_t() {}
		string_t(const std::string & l, const std::string & t) : lang(l), text(t) {}
		string_t(const string_t & s) : lang(s.lang), text(s.text) {}
		string_t & operator=(const string_t & s) {
			lang = s.lang;
			text = s.text;
			return *this;
		}
		string_t(string_t && s) : lang(std::move(s.lang)), text(std::move(s.text)) {}
	};
private:
	typedef std::vector<string_t> list_t;
public:
	typedef list_t::const_iterator const_iterator;
	typedef list_t::const_reverse_iterator const_reverse_iterator;
private:
	list_t list;
	string_t empty;
public:
	UserDataStringList() {}
	UserDataStringList(const UserDataStringList & l) : list(l.list) {}
	UserDataStringList & operator=(const UserDataStringList &l) {
		list = l.list;
		return *this;
	}
	UserDataStringList(UserDataStringList && l) : list(std::move(l.list)) {}
	size_t load(const void * p, size_t ofs, size_t len) {
		size_t rv = 0;
		for (size_t i = 0; i + 4 <= len;) {
			string_t e;
			uint16_t size = Box::big16(*reinterpret_cast<const uint16_t *>(reinterpret_cast<const uint8_t *>(p) + ofs + i));
			uint16_t lc = Box::big16(*reinterpret_cast<const uint16_t *>(reinterpret_cast<const uint8_t *>(p) + ofs + i + 2));
			if (lc < 0x400 || lc == 0x7FFF) {
				e.lang = "und";
				e.text.reserve(size);
				const uint8_t * q = reinterpret_cast<const uint8_t *>(p) + ofs + i + 4;
				for (size_t j = 0; j < size; ++j, ++q) {
					e.text.append(1, (char)(*q & 0x7F));
				}
			}
			else
			{
				char lbuf[4] = { 0 };
				lbuf[0] = (char)(0x60 + ((lc >> 10) & 0x1F));
				lbuf[1] = (char)(0x60 + ((lc >> 5) & 0x1F));
				lbuf[2] = (char)(0x60 + (lc & 0x1F));
				e.lang = lbuf;
				if (size >= 2 && Box::big16(*reinterpret_cast<const uint16_t *>(reinterpret_cast<const uint8_t *>(p) + ofs + i + 4)) == 0xFEFF)
				{
					e.text = Utf::utf8(Utf::utf16be(reinterpret_cast<const uint16_t *>(reinterpret_cast<const uint8_t *>(p) + ofs + i + 6), (size - 2) / 2));
				}
				else
				{
					e.text = Utf::utf8(Utf::utf16(reinterpret_cast<const char *>(p) + ofs + i + 4, size));
				}
			}
			list.push_back(e);
			i += size + 4;
			++rv;
		}
		return rv;
	}
	UserDataStringList(const void * p, size_t ofs, size_t len) {
		load(p, ofs, len);
	}
	size_t count() const {
		return list.size();
	}
	UserDataStringList & append(const string_t & s) {
		list.push_back(s);
		return *this;
	}
	const string_t * at(size_t index) const {
		if (index < list.size()) {
			return &list[index];
		}
		else
		{
			return nullptr;
		}
	}
	size_t find(const char * lang) const {
		size_t i = 0;
		for (const_iterator it = begin(); it != end(); ++it, ++i) {
			if (_stricmp(lang, it->lang.c_str()) == 0) {
				break;
			}
		}
		return i;
	}
	bool has(const char * lang) const {
		return find(lang) < count();
	}
	const string_t * of(const char * lang) const {
		return at(find(lang));
	}
	const string_t * operator[](size_t index) const {
		return at(index);
	}
	const string_t * operator[](const char * lang) const {
		return of(lang);
	}
	const_iterator begin() const {
		return list.begin();
	}
	const_iterator end() const {
		return list.end();
	}
	const_reverse_iterator rbegin() const {
		return list.rbegin();
	}
	const_reverse_iterator rend() const {
		return list.rend();
	}
	void json(Json::Value & arr) const {
		for (const_iterator it = begin(); it != end(); ++it) {
			Json::Value o(Json::objectValue);
			o["lang"] = it->lang;
			o["text"] = it->text;
			arr.append(o);
		}
	}
	Json::Value json() const {
		Json::Value arr(Json::arrayValue);
		json(arr);
		return arr;
	}
};

class UserDataStringBox : public Box {
private:
	UserDataStringList list;
public:
	UserDataStringBox(const void * p, size_t ofs) : Box(p, ofs) {
		list.load(dataptr(), 0, data_length());
	}
	const UserDataStringList & strings() const {
		return list;
	}
	UserDataStringList & strings() {
		return list;
	}
	virtual void json(Json::Value & obj) const {
		Box::json(obj);
		obj["strings"] = list.json();
	}
};

class UserDataStringBoxRegistry {
public:
	UserDataStringBoxRegistry(const char * strings, ...) {
		va_list ap;
		va_start(ap, strings);
		Box::box_allocator_t a = [](const void * p, size_t q) -> Box * { return new UserDataStringBox(p, q); };
		if (strings) {
			Box::registerBoxType(strings, a);
			for (const char * s = va_arg(ap, const char *); s && *s; s = va_arg(ap, const char *)) {
				Box::registerBoxType(s, a);
			}
		}
		va_end(ap);
	}
};

#endif

