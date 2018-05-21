#include "Utf.h"

std::string Utf::latin1(const char *p) {
	return std::string();
}

std::string Utf::utf8(const char * p, size_t n) {
	return std::string();
}

std::string Utf::utf8(const wchar_t *) {
	return std::string();
}

std::wstring Utf::utf16(const char *, size_t) {
	return std::wstring();
}

std::wstring Utf::utf16be(const uint16_t *, size_t) {
	return std::wstring();
}

size_t Utf::utf16be(const wchar_t *, uint16_t *)
{
	return 0;
}

