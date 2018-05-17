# MooCow

C++ command-line utility to work with boxes in ISO base media files


# Syntax

moocow (--parse | --format) --in infile --out outfile


| Option        | Description                                          |
|---------------|------------------------------------------------------|
| -p, --parse   | Read the boxes in the input ISO base media file and write to the output JSON file |
| -f, --format  | Read the boxes in the input JSON file and write to the output ISO base media file |
| -i, --in _f_  | Specify the input file name _f_                      |
| -o, --out _f_ | Specify the output file name _f_                     |


# Exit Status

Returns zero on success; non-zero on error. Human-readable error
messages written to stderr.


# Notes

* When parsing boxes, use the special output file name `stdout` to print
  to standard output.
* When formatting boxes, use the special input file name `stdin` to read
  from standard input.


# Building

* **Windows**: open and build `MooCow.sln` with Visual Studio 2017
* **Linux**: use `make` to build the supplied Makefile

_Build instructions for macOS are not available at this time._


# License

Copyright 2018 Michael Trenholm-Boyle

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.