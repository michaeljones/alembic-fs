
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


all: ../bin/alembic-fs

../bin: 
	mkdir -p ../bin

../bin/alembic-fs: ../bin ../obj/AlembicFS.o ../obj/wrap.o ../obj/main.o
	g++ -o ../bin/alembic-fs ../obj/* $(LINKFLAGS)

../obj:
	mkdir -p ../obj

../obj/AlembicFS.o: ../obj AlembicFS.cc AlembicFS.hh
	g++ $(CFLAGS) -c AlembicFS.cc -o ../obj/AlembicFS.o

../obj/main.o: ../obj main.cc wrap.hh
	g++ $(CFLAGS) -c main.cc -o ../obj/main.o

../obj/wrap.o: ../obj wrap.cc wrap.hh AlembicFS.hh
	g++ $(CFLAGS) -c wrap.cc -o ../obj/wrap.o

