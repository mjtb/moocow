#ifndef MOOCOW_MOVIEBOX_H_INCLUDED
#define MOOCOW_MOVIEBOX_H_INCLUDED

#include <ctime>

class MovieBox : public Box {
public:
	MovieBox(const void * p, size_t o) : Box(p, o)  {
		make_container();
		children()->load(dataptr(), 0, size() - header_length());
	}
};

class MovieHeaderBox : public FullBox {
public:
	MovieHeaderBox(const void * p, size_t o) : FullBox(p, o) {}
	static std::string date(uint64_t t) {
		time_t tt = 0;
		if (t >= 2082844800) {
			tt = (time_t)(t - 2082844800);
		}
		tm tb = *gmtime(&tt);
		char buf[30] = { 0 };
		snprintf(buf, sizeof(buf) - 1, "%04i-%02i-%02iT%02i:%02i:%02i.000Z", tb.tm_year + 1900, tb.tm_mon + 1, tb.tm_mday, tb.tm_hour, tb.tm_min, tb.tm_sec);
		return std::string(buf);
	}
	uint64_t creation_time() const {
		if (version() == 1) {
			return big64(*reinterpret_cast<const uint64_t *>(dataptr(0)));
		}
		else {
			return big32(*reinterpret_cast<const uint32_t *>(dataptr(0)));
		}
	}
	uint64_t modification_time() const {
		if (version() == 1) {
			return big64(*reinterpret_cast<const uint64_t *>(dataptr(8)));
		}
		else {
			return big32(*reinterpret_cast<const uint32_t *>(dataptr(4)));
		}
	}
	uint32_t timescale() const {
		if (version() == 1) {
			return big32(*reinterpret_cast<const uint32_t *>(dataptr(16)));
		}
		else {
			return big32(*reinterpret_cast<const uint32_t *>(dataptr(8)));
		}
	}
	uint64_t duration() const {
		if (version() == 1) {
			return big64(*reinterpret_cast<const uint64_t *>(dataptr(20)));
		}
		else {
			return big32(*reinterpret_cast<const uint32_t *>(dataptr(12)));
		}
	}
	double seconds(double x) const {
		double ts = timescale();
		double nts = pow(10, ceil(log10(ts)));
		double rv = round(x / ts * nts) / nts;
		return rv;
	}
	double seconds() const {
		return seconds((double)duration());
	}
	double rate() const {
		double n;
		if (version() == 1) {
			n = big32(*reinterpret_cast<const uint32_t *>(dataptr(28)));
		}
		else {
			n = big32(*reinterpret_cast<const uint32_t *>(dataptr(16)));
		}
		return round(n / 65536 * 1e5) / 1e5;
	}
	double volume() const {
		double n;
		if (version() == 1) {
			n = big16(*reinterpret_cast<const uint32_t *>(dataptr(32)));
		}
		else {
			n = big16(*reinterpret_cast<const uint32_t *>(dataptr(20)));
		}
		return round(n / 256 * 1e3) / 1e3;
	}
	union matrix_t {
		struct matrix_component_t {
			double a, b, u, c, d, v, x, y, w;
		} cc;
		double m[9];
	};
	matrix_t matrix() const {
		matrix_t mat = { 0 };
		size_t start = (version() == 1) ? 44 : 32;
		for (size_t i = 0; i < 3; ++i) {
			for (size_t j = 0; j < 3; ++j) {
				double n = big32(*reinterpret_cast<const uint32_t *>(dataptr(start + 4 * (3 * i + j))));
				double tz = (j == 2) ? 1e10 : 1e5;
				mat.m[i * 3 + j] = n / ((j == 2) ? 2147483648 : 65536);
			}
		}
		return mat;
	}
	uint32_t next_track_ID() const {
		return big32(*reinterpret_cast<const uint32_t *>(dataptr(((version() == 1) ? 44 : 32) + 9 * 4 + 6 * 4)));
	}
	virtual void json(Json::Value & obj) const {
		obj["creation_time"] = date(creation_time());
		obj["modification_time"] = date(modification_time());
		obj["timescale"] = timescale();
		obj["duration"] = duration();
		obj["seconds"] = seconds();
		obj["rate"] = rate();
		obj["volume"] = volume();
		matrix_t mat = matrix();
		Json::Value m(Json::arrayValue);
		for (size_t i = 0; i < 3; ++i) {
			Json::Value n(Json::arrayValue);
			for (size_t j = 0; j < 3; ++j) {
				n.append(mat.m[i * 3 + j]);
			}
			m.append(n);
		}
		obj["matrix"] = m;
		obj["next_track_ID"] = next_track_ID();
	}
};


#endif

