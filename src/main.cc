
#include "AlembicFS.hh"
#include "wrap.hh"

#include "JsonValuesPropertyView.hh"
#include "MaxValuePropertyView.hh"
#include "MinValuePropertyView.hh"
#include "ValuesPropertyView.hh"

#include <boost/scoped_ptr.hpp>

#include <fuse.h>
#include <stdio.h>


struct fuse_operations alembicfs_oper;

int main(int argc, char *argv[])
{
    // Set appropriate call backs
    alembicfs_oper.getattr = wrap_getattr;
    alembicfs_oper.readlink = wrap_readlink;
    alembicfs_oper.getdir = NULL;
    alembicfs_oper.mknod = wrap_mknod;
    alembicfs_oper.mkdir = wrap_mkdir;
    alembicfs_oper.unlink = wrap_unlink;
    alembicfs_oper.rmdir = wrap_rmdir;
    alembicfs_oper.symlink = wrap_symlink;
    alembicfs_oper.rename = wrap_rename;
    alembicfs_oper.link = wrap_link;
    alembicfs_oper.chmod = wrap_chmod;
    alembicfs_oper.chown = wrap_chown;
    alembicfs_oper.truncate = wrap_truncate;
    alembicfs_oper.utime = wrap_utime;
    alembicfs_oper.open = wrap_open;
    alembicfs_oper.read = wrap_read;
    alembicfs_oper.write = wrap_write;
    alembicfs_oper.statfs = wrap_statfs;
    alembicfs_oper.flush = wrap_flush;
    alembicfs_oper.release = wrap_release;
    alembicfs_oper.fsync = wrap_fsync;
    alembicfs_oper.readdir = wrap_readdir;
    alembicfs_oper.releasedir = wrap_releasedir;
    alembicfs_oper.fsyncdir = wrap_fsyncdir;
    alembicfs_oper.init = wrap_init;

    printf("mounting file system...\n");

    // Stat the alembic file to give us some base information to work with
    //
    const char* path = argv[argc-1];
    --argc;
    struct stat abcStat;
    lstat(path, &abcStat);

    boost::scoped_ptr< Alembic::AbcGeom::IArchive > archive(
            new Alembic::AbcGeom::IArchive(
                Alembic::AbcCoreHDF5::ReadArchive(),
                path,
                Alembic::AbcGeom::ErrorHandler::kQuietNoopPolicy
                )
            );

    PropertyViewLookup propertyViewLookup;

    boost::scoped_ptr< PropertyView > valuesPropertyView( new ValuesPropertyView );
    propertyViewLookup[ "values" ] = valuesPropertyView.get();

    boost::scoped_ptr< PropertyView > minValuePropertyView( new MinValuePropertyView );
    propertyViewLookup[ "min" ] = minValuePropertyView.get();

    boost::scoped_ptr< PropertyView > maxValuePropertyView( new MaxValuePropertyView );
    propertyViewLookup[ "max" ] = maxValuePropertyView.get();

    boost::scoped_ptr< PropertyView > jsonValuesPropertyView( new JsonValuesPropertyView );
    propertyViewLookup[ "json" ] = jsonValuesPropertyView.get();

    boost::scoped_ptr< AlembicFS > instance(
            new AlembicFS(
                "/",        // Set our root - is this needed?
                &abcStat,
                path,
                archive.get(),
                propertyViewLookup
                )
            );
    AlembicFS::setInstance( instance.get() );

    int fuse_stat = fuse_main(argc, argv, &alembicfs_oper, NULL);

    printf("fuse_main returned %d\n", fuse_stat);

    return fuse_stat;
}


