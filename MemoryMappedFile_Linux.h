#ifndef MOOCOW_MEMORYMAPPEDFILE_H_INCLUDED
#define MOOCOW_MEMORYMAPPEDFILE_H_INCLUDED

#ifdef _MSC_VER
#error This file is Linux-specific.
#endif

#include <string>
#include <cerrno>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

class MemoryMappedFile
{
private:
	int fd;
	void * addr;
	size_t  length;
	int statusCode;
	std::string statusText;
	const char * opname;
public:
	MemoryMappedFile() : addr(MAP_FAILED), statusCode(-1), length(0), fd(-1), opname(nullptr) {}
	int status() const {
		return statusCode;
	}
	const char * failureReason() const {
		return statusText.c_str();
	}
	const char * failedOperation() const {
		return opname;
	}
	const void * baseptr() const {
		return addr != MAP_FAILED ? addr : nullptr;
	}
	size_t size() const {
		return length;
	}
	bool open(const char * filename) {
		fd = ::open(filename, O_RDONLY);
		if (fd == -1) {
			statusCode = errno;
			statusText = std::string(strerror(statusCode));
			opname = "open";
			return false;
		}
		struct stat st = { 0 };
		if (fstat(fd, &st)) {
			statusCode = errno;
			statusText = std::string(strerror(statusCode));
			opname = "fstat";
			return false;
		}
		length = static_cast<size_t>(st.st_size);
		addr = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
		if (addr == MAP_FAILED) {
			statusCode = errno;
			statusText = std::string(strerror(statusCode));
			opname = "mmap";
			return false;
		}
		return true;
	}
	void close() {
		if (addr != MAP_FAILED) {
			munmap(addr, length);
			addr = MAP_FAILED;
		}
		if (fd != -1) {
			::close(fd);
			fd = -1;
		}
	}
	~MemoryMappedFile() {
		close();
	}
};

#endif
