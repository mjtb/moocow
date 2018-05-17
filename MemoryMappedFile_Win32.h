#ifndef MOOCOW_MEMORYMAPPEDFILE_H_INCLUDED
#define MOOCOW_MEMORYMAPPEDFILE_H_INCLUDED

#ifndef _MSC_VER
#error This file is Win32-specific.
#endif

#include <cstdint>
#include <string>
#include <limits>

#include <Windows.h>

class MemoryMappedFile
{
private:
	int statusCode;
	const char * opname;
	std::string statusText;
	uint64_t fileSize;
	HANDLE hFile;
	HANDLE hMap;
	LPVOID pView;
	static std::string FormatStatusText(int statusCode)
	{
		LPVOID pBuf = nullptr;
		std::string rv;
		int nv = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | 72, nullptr, static_cast<DWORD>(statusCode), 0, reinterpret_cast<LPSTR>(&pBuf), 0, nullptr);
		if (pBuf && nv)
		{
			rv = std::string(reinterpret_cast<const char *>(pBuf), reinterpret_cast<const char *>(pBuf) + nv);
			LocalFree(pBuf);
		}
		else
		{
			rv = "an unknown error occured";
		}
		return rv;
	}
public:
	MemoryMappedFile() : statusCode(0), fileSize(0), opname(nullptr), hFile(INVALID_HANDLE_VALUE), hMap(nullptr), pView(nullptr) {}
	const int status() const {
		return statusCode;
	}
	const char * failureReason() const {
		return statusCode ? statusText.c_str() : nullptr;
	}
	const char * failedOperation() const {
		return opname;
	}
	bool open(const char * filename) {
		BOOL bResult;

		hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (!hFile || (hFile == INVALID_HANDLE_VALUE)) {
			statusCode = GetLastError();
			statusText = FormatStatusText(statusCode);
			opname = "CreateFileA";
			return false;
		}
		bResult = GetFileSizeEx(hFile, reinterpret_cast<PLARGE_INTEGER>(&fileSize));
		if (!bResult) {
			statusCode = GetLastError();
			statusText = FormatStatusText(statusCode);
			opname = "GetFileSizeEx";
			return false;
		}
		hMap = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
		if (!hMap) {
			statusCode = GetLastError();
			statusText = FormatStatusText(statusCode);
			opname = "CreateFileMapping";
			return false;
		}
		pView = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
		if (!pView) {
			statusCode = GetLastError();
			statusText = FormatStatusText(statusCode);
			opname = "MapViewOfFile";
			return false;
		}
		return true;
	}
	void close() {
		if (pView) {
			UnmapViewOfFile(pView);
			pView = nullptr;
		}
		if (hMap) {
			CloseHandle(hMap);
			hMap = nullptr;
		}
		if (hFile && (hFile != INVALID_HANDLE_VALUE)) {
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}
	}
	~MemoryMappedFile() {
		close();
	}
	const void * baseptr() const {
		return pView;
	}
	size_t size() const {
		return static_cast<size_t>(std::min<uint64_t>(fileSize, std::numeric_limits<size_t>::max() - 128));
	}
};

#endif
