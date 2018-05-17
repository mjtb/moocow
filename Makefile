objects = obj/Box.o obj/FileTypeBox.o obj/jsoncpp.o obj/MooCow.o
CFLAGS = -std=c++11

dist: dist/bin/moocow

dist/bin/moocow: $(objects)
	mkdir -p dist/bin
	g++ -o dist/bin/moocow $(objects)

clean: 
	rm -rf dist obj

obj:
	mkdir -p obj

obj/jsoncpp.o: obj json.h jsoncpp.cpp
	g++ $(CFLAGS) -c -o obj/jsoncpp.o jsoncpp.cpp

obj/Box.o: obj/jsoncpp.o Box.h Box.cpp
	g++ $(CFLAGS) -c -o obj/Box.o Box.cpp

obj/FileTypeBox.o: obj/Box.o FileTypeBox.h FileTypeBox.cpp
	g++ $(CFLAGS) -c -o obj/FileTypeBox.o FileTypeBox.cpp

obj/MooCow.o: obj/Box.o obj/FileTypeBox.o MemoryMappedFile.h MemoryMappedFile_Linux.h CliOpts.h getopt.h MooCow.h MooCow.cpp
	g++ $(CFLAGS) -c -o obj/MooCow.o MooCow.cpp
