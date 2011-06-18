#include "wrap.hh"

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
    alembicfs_oper.setxattr = wrap_setxattr;
    alembicfs_oper.getxattr = wrap_getxattr;
    alembicfs_oper.listxattr = wrap_listxattr;
    alembicfs_oper.removexattr = wrap_removexattr;
    alembicfs_oper.opendir = wrap_opendir;
    alembicfs_oper.readdir = wrap_readdir;
    alembicfs_oper.releasedir = wrap_releasedir;
    alembicfs_oper.fsyncdir = wrap_fsyncdir;
    alembicfs_oper.init = wrap_init;

    printf("mounting file system...\n");

    int fuse_stat = fuse_main(argc, argv, &alembicfs_oper, NULL);

    printf("fuse_main returned %d\n", fuse_stat);

    return fuse_stat;
}


