#ifndef MOOCOW_UTF_H_INCLUDED
#define MOOCOW_UTF_H_INCLUDED

#include <string>

class Utf {
public:
	static std::string latin1(const char *);
	static std::string utf8(const char *, size_t);
	static std::string utf8(const std::string & s) {
		return utf8(s.c_str(), s.length());
	}
	static std::string utf8(const wchar_t *);
	static std::string utf8(const std::wstring & w) {
		return utf8(w.c_str());
	}
	static std::wstring utf16(const char *, size_t);
	static std::wstring utf16(const std::string & s) {
		return utf16(s.c_str(), s.length());
	}
	static std::wstring utf16be(const uint16_t *, size_t);
	static size_t utf16be(const wchar_t *, uint16_t *);
};

#endif
