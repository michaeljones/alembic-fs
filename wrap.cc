#include "wrap.hh"
#include "examplefs.hh"

void set_rootdir(const char *path) {
    AlembicFS::Instance()->setRootDir(path);
}

int wrap_getattr(const char *path, struct stat *statbuf) {
    return AlembicFS::Instance()->getattr(path, statbuf);
}

int wrap_readlink(const char *path, char *link, size_t size) {
    return AlembicFS::Instance()->readlink(path, link, size);
}

int wrap_mknod(const char *path, mode_t mode, dev_t dev) {
    return AlembicFS::Instance()->mknod(path, mode, dev);
}
int wrap_mkdir(const char *path, mode_t mode) {
    return AlembicFS::Instance()->mkdir(path, mode);
}
int wrap_unlink(const char *path) {
    return AlembicFS::Instance()->unlink(path);
}
int wrap_rmdir(const char *path) {
    return AlembicFS::Instance()->rmdir(path);
}
int wrap_symlink(const char *path, const char *link) {
    return AlembicFS::Instance()->symlink(path, link);
}
int wrap_rename(const char *path, const char *newpath) {
    return AlembicFS::Instance()->rename(path, newpath);
}
int wrap_link(const char *path, const char *newpath) {
    return AlembicFS::Instance()->link(path, newpath);
}
int wrap_chmod(const char *path, mode_t mode) {
    return AlembicFS::Instance()->chmod(path, mode);
}
int wrap_chown(const char *path, uid_t uid, gid_t gid) {
    return AlembicFS::Instance()->chown(path, uid, gid);
}
int wrap_truncate(const char *path, off_t newSize) {
    return AlembicFS::Instance()->truncate(path, newSize);
}
int wrap_utime(const char *path, struct utimbuf *ubuf) {
    return AlembicFS::Instance()->utime(path, ubuf);
}
int wrap_open(const char *path, struct fuse_file_info *fileInfo) {
    return AlembicFS::Instance()->open(path, fileInfo);
}
int wrap_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    return AlembicFS::Instance()->read(path, buf, size, offset, fileInfo);
}
int wrap_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    return AlembicFS::Instance()->write(path, buf, size, offset, fileInfo);
}
int wrap_statfs(const char *path, struct statvfs *statInfo) {
    return AlembicFS::Instance()->statfs(path, statInfo);
}
int wrap_flush(const char *path, struct fuse_file_info *fileInfo) {
    return AlembicFS::Instance()->flush(path, fileInfo);
}
int wrap_release(const char *path, struct fuse_file_info *fileInfo) {
    return AlembicFS::Instance()->release(path, fileInfo);
}
int wrap_fsync(const char *path, int datasync, struct fuse_file_info *fi) {
    return AlembicFS::Instance()->fsync(path, datasync, fi);
}
int wrap_setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
    return AlembicFS::Instance()->setxattr(path, name, value, size, flags);
}
int wrap_getxattr(const char *path, const char *name, char *value, size_t size) {
    return AlembicFS::Instance()->getxattr(path, name, value, size);
}
int wrap_listxattr(const char *path, char *list, size_t size) {
    return AlembicFS::Instance()->listxattr(path, list, size);
}
int wrap_removexattr(const char *path, const char *name) {
    return AlembicFS::Instance()->removexattr(path, name);
}
int wrap_opendir(const char *path, struct fuse_file_info *fileInfo) {
    return AlembicFS::Instance()->opendir(path, fileInfo);
}
int wrap_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    return AlembicFS::Instance()->readdir(path, buf, filler, offset, fileInfo);
}
int wrap_releasedir(const char *path, struct fuse_file_info *fileInfo) {
    return AlembicFS::Instance()->releasedir(path, fileInfo);
}
int wrap_fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
    return AlembicFS::Instance()->fsyncdir(path, datasync, fileInfo);
}
void* wrap_init(struct fuse_conn_info *conn) {
    return AlembicFS::Instance()->init(conn);
}

