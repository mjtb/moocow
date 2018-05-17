objects = obj/jsoncpp.o obj/MooCow.o
CFLAGS = -std=c++11

dist: dist/bin/moocow

dist/bin/moocow: $(objects)
	mkdir -p dist/bin
	g++ -o dist/bin/moocow $(objects)

clean:
	rm -rf dist obj

obj/jsoncpp.o:	json.h jsoncpp.cpp
	mkdir -p obj
	g++ $(CFLAGS) -c -o obj/jsoncpp.o jsoncpp.cpp

obj/MooCow.o:	json.h MemoryMappedFile.h MemoryMappedFile_Linux.h CliOpts.h MooCow.h MooCow.cpp \
		Box.h FileTypeBox.h
	mkdir -p obj
	g++ $(CFLAGS) -c -o obj/MooCow.o MooCow.cpp
