
CFLAGS = -Wall -DFUSE_USE_VERSION=26 `pkg-config fuse --cflags`
LINKFLAGS = -Wall `pkg-config fuse --libs`

all: bin/examplefs

clean:
	rm -rf bin obj

bin: 
	mkdir -p bin

bin/examplefs: bin obj/examplefs.o obj/wrap.o obj/main.o
	g++ -g $(LINKFLAGS) -o bin/examplefs obj/*

obj:
	mkdir -p obj

obj/examplefs.o: obj examplefs.cc examplefs.hh
	g++ -g $(CFLAGS) -c examplefs.cc -o obj/examplefs.o

obj/main.o: obj main.c wrap.hh
	gcc -g $(CFLAGS) -c main.c -o obj/main.o

obj/wrap.o: obj wrap.cc wrap.hh examplefs.hh
	g++ -g $(CFLAGS) -c wrap.cc -o obj/wrap.o

