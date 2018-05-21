#include "stdafx.h"

#include "Box.h"
#include "Utf.h"

std::string Utf::utf8(const char * p, size_t n) {
	int wbuf = MultiByteToWideChar(28591, 0, p, n, nullptr, 0); // ISO 8859-1 Latin 1
	if (wbuf < 1) {
		return std::string();
	}
	std::unique_ptr<wchar_t[]> buf(new wchar_t[wbuf + 2]);
	memset(buf.get(), 0, sizeof(wchar_t) * (wbuf + 2));
	MultiByteToWideChar(28591, 0, p, n, buf.get(), wbuf + 1);
	return utf8(buf.get());
}

std::string Utf::latin1(const char *p) {
	std::wstring w = utf16(p);
	int cbuf = WideCharToMultiByte(28591, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr); // ISO 8859-1 Latin 1
	if (cbuf < 1) {
		return std::string();
	}
	std::unique_ptr<char[]> buf(new char[cbuf + 2]);
	memset(buf.get(), 0, cbuf + 2);
	WideCharToMultiByte(28591, 0, w.c_str(), -1, buf.get(), cbuf + 1, nullptr, nullptr);
	return std::string(buf.get());
}


std::string Utf::utf8(const wchar_t * p) {
	int cbuf = WideCharToMultiByte(CP_UTF8, 0, p, -1, nullptr, 0, nullptr, nullptr);
	if (cbuf < 1) {
		return std::string();
	}
	std::unique_ptr<char[]> buf(new char[cbuf + 2]);
	memset(buf.get(), 0, cbuf + 2);
	WideCharToMultiByte(CP_UTF8, 0, p, -1, buf.get(), cbuf + 1, nullptr, nullptr);
	return std::string(buf.get());
}

std::wstring Utf::utf16(const char * p, size_t n) {
	int wbuf = MultiByteToWideChar(CP_UTF8, 0, p, (int)n, nullptr, 0);
	if (wbuf < 1) {
		return std::wstring();
	}
	std::unique_ptr<wchar_t[]> buf(new wchar_t[wbuf + 2]);
	memset(buf.get(), 0, sizeof(wchar_t) * (wbuf + 2));
	MultiByteToWideChar(CP_UTF8, 0, p, (int)n, buf.get(), wbuf + 1);
	return std::wstring(buf.get());
}

std::wstring Utf::utf16be(const uint16_t *p, size_t n) {
	if (n == 0) {
		return std::wstring();
	}
	std::unique_ptr<wchar_t[]> buf(new wchar_t[n + 1]);
	memset(buf.get(), 0, sizeof(wchar_t) * (n + 1));
	for (size_t i = 0; i < n && p[i]; ++i) {
		buf.get()[i] = Box::big16(p[i]);
	}
	return std::wstring(buf.get());
}

size_t Utf::utf16be(const wchar_t * p, uint16_t * q)
{
	size_t i;
	for (i = 0; p[i]; ++i)
	{
		q[i] = Box::big16(static_cast<uint16_t>(p[i] & 0xFFFF));
	}
	return i;
}

