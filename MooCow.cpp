// MooCow.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#ifndef MOOCOW_H_STATIC_DEFINES
#define MOOCOW_H_STATIC_DEFINES
#endif

#include "MooCow.h"
#include "CliOpts.h"
#include "MemoryMappedFile.h"

static int parse(const CliOpts & opts);
static int format(const CliOpts & opts);

int main(int argc, char* const* argv)
{
	CliOpts opts(argc, argv);
	int rv = opts.status();
	switch (opts.action()) {
	default:
		break;
	case CliOpts::parse_boxes:
		rv = parse(opts);
		break;
	case CliOpts::format_boxes:
		rv = format(opts);
		break;
	}
	return rv;
}

static int parse(const CliOpts & opts) {

	MemoryMappedFile mf;
	if (!mf.open(opts.infile())) {
		std::cerr
			<< "error: cannot open file: opts.infile()" << std::endl
			<< "       " << mf.failedOperation() << " returned " << mf.status() << ": " << std::endl
			<< "       " << mf.failureReason() << std::endl;
		return 1;
	}
	else
	{
		Boxes boxes(mf.baseptr(), 0, mf.size());
		Json::Value arr(Json::arrayValue);
		for(size_t i = 0, n = boxes.count(); i < n; ++i) {
			arr.append(boxes[i]->json());
		}
		std::unique_ptr<std::ofstream> osf;
		std::ostream * os = nullptr;
		if (strcmp(opts.outfile(), "stdout") == 0) {
			os = &std::cout;
		}
		else
		{
			osf.reset(new std::ofstream(opts.outfile(), std::ofstream::trunc | std::ofstream::out));
			os = osf.get();
		}
		(*os) << arr << std::endl;
		return 0;
	}
}

int format(const CliOpts & opts) {
	std::cerr << "error: not implemented: --format" << std::endl;
	CliOpts::syntax(std::cerr);
	return 1;
}

