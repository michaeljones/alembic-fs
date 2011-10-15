
CFLAGS = -Wall -DFUSE_USE_VERSION=26 `pkg-config fuse --cflags` \
		 -I$(TOOLS)/include/alembic/1.0.2 \
		 -I$(TOOLS)/include/boost/1_44_0 \
		 -I$(TOOLS)/include/OpenEXR \
		 -I$(TOOLS)/include/hdf5/1_8_7 \

LINKFLAGS = -Wall `pkg-config fuse --libs` \
			-L$(TOOLS)/lib/alembic/1.0.2/static \
			-L$(TOOLS)/lib/hdf5/1_8_7 \
			-L$(TOOLS)/lib \
			-lAlembicAbcGeom \
			-lAlembicAbc \
			-lAlembicAbcCoreHDF5 \
			-lAlembicAbcCoreAbstract \
			-lAlembicUtil \
			-lhdf5_hl \
			-lhdf5 \
			-lz \
			-lHalf \


all: bin/alembic-fs

clean:
	rm -rf bin obj

bin: 
	mkdir -p bin

bin/alembic-fs: bin obj/AlembicFS.o obj/wrap.o obj/main.o
	g++ -o bin/alembic-fs obj/* $(LINKFLAGS)

obj:
	mkdir -p obj

obj/AlembicFS.o: obj src/AlembicFS.cc src/AlembicFS.hh
	g++ $(CFLAGS) -c src/AlembicFS.cc -o obj/AlembicFS.o

obj/main.o: obj src/main.c src/wrap.hh
	g++ $(CFLAGS) -c src/main.c -o obj/main.o

obj/wrap.o: obj src/wrap.cc src/wrap.hh src/AlembicFS.hh
	g++ $(CFLAGS) -c src/wrap.cc -o obj/wrap.o

