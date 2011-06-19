
CFLAGS = -Wall -DFUSE_USE_VERSION=26 `pkg-config fuse --cflags`
LINKFLAGS = -Wall `pkg-config fuse --libs`

all: bin/alembic-fs

clean:
	rm -rf bin obj

bin: 
	mkdir -p bin

bin/alembic-fs: bin obj/AlembicFS.o obj/wrap.o obj/main.o
	g++ -g $(LINKFLAGS) -o bin/alembic-fs obj/*

obj:
	mkdir -p obj

obj/AlembicFS.o: obj AlembicFS.cc AlembicFS.hh
	g++ -g $(CFLAGS) -c AlembicFS.cc -o obj/AlembicFS.o

obj/main.o: obj main.c wrap.hh
	g++ -g $(CFLAGS) -c main.c -o obj/main.o

obj/wrap.o: obj wrap.cc wrap.hh AlembicFS.hh
	g++ -g $(CFLAGS) -c wrap.cc -o obj/wrap.o

