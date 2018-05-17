#ifndef MOOCOW_CLIOPTS_H_INCLUDED
#define MOOCOW_CLIOPTS_H_INCLUDED

#include <string>
#include <iostream>

#ifdef _MSC_VER
#include "getopt.h"
#else
#include <getopt.h>
#endif

class CliOpts
{
public:
	enum Action {
		no_action,
		parse_boxes,
		format_boxes
	};
private:
	std::string inarg;
	bool has_inarg;
	std::string outarg;
	bool has_outarg;
	int rv;
	Action action_val;
public:
	CliOpts(int argc, char * const * argv) :
		has_inarg(false),
		has_outarg(false),
		action_val(no_action),
		rv(0) {
		struct option longopts[] = {
			{ "in", required_argument, nullptr, 'i' },
			{ "out", required_argument, nullptr, 'o' },
			{ "parse", no_argument, nullptr, 'p' },
			{ "format", no_argument, nullptr, 'f' },
			{ "help", no_argument, nullptr, '?' },
			{ 0 }
		};
		int index = 0;
		bool loop = true, help = false;
		while (loop)
		{
			switch (getopt_long(argc, argv, "i:o:pfh", longopts, &index))
			{
			default:
				rv = 1;
				help = true;
				break;
			case -1:
				loop = false;
				break;
			case 'i':
				inarg = optarg;
				has_inarg = true;
				break;
			case 'o':
				outarg = optarg;
				has_outarg = true;
				break;
			case 'p':
				action_val = parse_boxes;
				break;
			case 'f':
				action_val = format_boxes;
				break;
			case 'h':
				action_val = no_action;
				help = true;
				break;
			}
		}
		if (optind < argc) {
			int oi = optind;
			inarg = argv[oi];
			has_inarg = true;
			++oi;
			if (oi < argc) {
				outarg = argv[oi];
				has_outarg = true;
			}
		}
		if (!help && (action_val == no_action))
		{
			if (has_inarg)
			{
				if (!has_outarg)
				{
					outarg = "stdout";
					has_outarg = true;
				}
				action_val = parse_boxes;
			}
			else
			{
				help = true;
				rv = 1;
			}
		}
		if (help)
		{
			syntax();
		}
	}
	const char * infile(const char * defval = nullptr) const {
		return has_inarg ? inarg.c_str() : defval;
	}
	const char * outfile(const char * defval = nullptr) const {
		return has_outarg ? outarg.c_str() : defval;
	}
	Action action() const {
		return action_val;
	}
	int status() const {
		return rv;
	}
	static void syntax(std::ostream & os) {
		//    "123456789012345678901234567890123456789012345678901234567890123456789012"
		os << "# MooCow"                                                                 << std::endl << std::endl;
		os << "Work with boxes in ISO base media files"                                  << std::endl << std::endl << std::endl;
		os << "# Syntax"                                                                 << std::endl << std::endl;
		os << "moocow (--parse | --format) --in infile --out outfile"                    << std::endl << std::endl << std::endl;
		os << "| Option        | Description                                          |" << std::endl;
		os << "|---------------|------------------------------------------------------|" << std::endl;
		os << "| -p, --parse   | Read the boxes in the input ISO base media file and"    << std::endl
		   << "                  write to the output JSON file                        |" << std::endl;
		os << "| -f, --format  | Read the boxes in the input JSON file and write to"     << std::endl
		   << "                  the output ISO base media file                       |" << std::endl;
		os << "| -i, --in _f_  | Specify the input file name _f_                      |" << std::endl;
		os << "| -o, --out _f_ | Specify the output file name _f_                     |" << std::endl << std::endl << std::endl;
		os << "# Exit Status"                                                            << std::endl << std::endl;
		os << "Returns zero on success; non-zero on error. Human-readable error"         << std::endl
		   << "messages written to stderr."                                              << std::endl << std::endl << std::endl;
		os << "# Notes"                                                                  << std::endl << std::endl;
		os << "* When parsing boxes, use the special output file name `stdout` to print" << std::endl
		   << "  to standard output."                                                    << std::endl;
		os << "* When formatting boxes, use the special input file name `stdin` to read" << std::endl
		   << "  from standard input."                                                   << std::endl;
	}
	static void syntax() {
		syntax(std::cout);
	}
};

#endif
