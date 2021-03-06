#ifndef ALEMBICFS_HH
#define ALEMBICFS_HH

#include "ClassifiedObject.hh"
#include "PropertyData.hh"
#include "PropertyView.hh"

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcGeom/All.h>

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>

class AlembicFS
{
public:

    AlembicFS(
            const char* root,
            struct stat* statData,
            const char* path,
            Alembic::AbcGeom::IArchive* archive,
            PropertyViewLookup& propertyViewLookup
            );

    static AlembicFS* instance();
    static void setInstance( AlembicFS* instance );

public:

    int getattr(const char *path, struct stat *statbuf);
    int readlink(const char *path, char *link, size_t size);
    int mknod(const char *path, mode_t mode, dev_t dev);
    int mkdir(const char *path, mode_t mode);
    int unlink(const char *path);
    int rmdir(const char *path);
    int symlink(const char *path, const char *link);
    int rename(const char *path, const char *newpath);
    int link(const char *path, const char *newpath);
    int chmod(const char *path, mode_t mode);
    int chown(const char *path, uid_t uid, gid_t gid);
    int truncate(const char *path, off_t newSize);
    int utime(const char *path, struct utimbuf *ubuf);
    int open(const char *path, struct fuse_file_info *fileInfo);
    int read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
    int write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
    int statfs(const char *path, struct statvfs *statInfo);
    int flush(const char *path, struct fuse_file_info *fileInfo);
    int release(const char *path, struct fuse_file_info *fileInfo);
    int fsync(const char *path, int datasync, struct fuse_file_info *fi);
    int setxattr(const char *path, const char *name, const char *value, size_t size, int flags);
    int getxattr(const char *path, const char *name, char *value, size_t size);
    int listxattr(const char *path, char *list, size_t size);
    int removexattr(const char *path, const char *name);
    int readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo);
    int releasedir(const char *path, struct fuse_file_info *fileInfo);
    int fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo);
    int truncate(const char *path, off_t offset, struct fuse_file_info *fileInfo);
    void* init(struct fuse_conn_info *conn);

private:

    static AlembicFS* s_instance;

private:

    const char* m_root;
    struct stat* m_stat;
    std::string m_path;
    Alembic::AbcGeom::IArchive* m_archive;

    PropertyViewLookup m_propertyViewLookup;

private:

    void absPath(char dest[PATH_MAX], const char *path);
    ClassifiedObject getObjectFromPath( const char* path );

    PropertyData getPropertyDataFromPath(
            Alembic::AbcGeom::IObject iObj,
            const PathSegments& path
            );
};



#endif // ALEMBICFS_HH
