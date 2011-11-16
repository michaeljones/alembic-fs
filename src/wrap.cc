#include "wrap.hh"
#include "AlembicFS.hh"

void set_rootdir(const char *path) {
    AlembicFS::Instance()->setRootDir(path);
}

int wrap_getattr(const char *path, struct stat *statbuf) {
    return AlembicFS::instance()->getattr(path, statbuf);
}

int wrap_readlink(const char *path, char *link, size_t size) {
    return AlembicFS::instance()->readlink(path, link, size);
}

int wrap_mknod(const char *path, mode_t mode, dev_t dev) {
    return AlembicFS::instance()->mknod(path, mode, dev);
}
int wrap_mkdir(const char *path, mode_t mode) {
    return AlembicFS::instance()->mkdir(path, mode);
}
int wrap_unlink(const char *path) {
    return AlembicFS::instance()->unlink(path);
}
int wrap_rmdir(const char *path) {
    return AlembicFS::instance()->rmdir(path);
}
int wrap_symlink(const char *path, const char *link) {
    return AlembicFS::instance()->symlink(path, link);
}
int wrap_rename(const char *path, const char *newpath) {
    return AlembicFS::instance()->rename(path, newpath);
}
int wrap_link(const char *path, const char *newpath) {
    return AlembicFS::instance()->link(path, newpath);
}
int wrap_chmod(const char *path, mode_t mode) {
    return AlembicFS::instance()->chmod(path, mode);
}
int wrap_chown(const char *path, uid_t uid, gid_t gid) {
    return AlembicFS::instance()->chown(path, uid, gid);
}
int wrap_truncate(const char *path, off_t newSize) {
    return AlembicFS::instance()->truncate(path, newSize);
}
int wrap_utime(const char *path, struct utimbuf *ubuf) {
    return AlembicFS::instance()->utime(path, ubuf);
}
int wrap_open(const char *path, struct fuse_file_info *fileInfo) {
    return AlembicFS::instance()->open(path, fileInfo);
}
int wrap_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    return AlembicFS::instance()->read(path, buf, size, offset, fileInfo);
}
int wrap_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    return AlembicFS::instance()->write(path, buf, size, offset, fileInfo);
}
int wrap_statfs(const char *path, struct statvfs *statInfo) {
    return AlembicFS::instance()->statfs(path, statInfo);
}
int wrap_flush(const char *path, struct fuse_file_info *fileInfo) {
    return AlembicFS::instance()->flush(path, fileInfo);
}
int wrap_release(const char *path, struct fuse_file_info *fileInfo) {
    return AlembicFS::instance()->release(path, fileInfo);
}
int wrap_fsync(const char *path, int datasync, struct fuse_file_info *fi) {
    return AlembicFS::instance()->fsync(path, datasync, fi);
}
int wrap_setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
    return AlembicFS::instance()->setxattr(path, name, value, size, flags);
}
int wrap_getxattr(const char *path, const char *name, char *value, size_t size) {
    return AlembicFS::instance()->getxattr(path, name, value, size);
}
int wrap_listxattr(const char *path, char *list, size_t size) {
    return AlembicFS::instance()->listxattr(path, list, size);
}
int wrap_removexattr(const char *path, const char *name) {
    return AlembicFS::instance()->removexattr(path, name);
}
int wrap_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    return AlembicFS::instance()->readdir(path, buf, filler, offset, fileInfo);
}
int wrap_releasedir(const char *path, struct fuse_file_info *fileInfo) {
    return AlembicFS::instance()->releasedir(path, fileInfo);
}
int wrap_fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
    return AlembicFS::instance()->fsyncdir(path, datasync, fileInfo);
}
void* wrap_init(struct fuse_conn_info *conn) {
    return AlembicFS::instance()->init(conn);
}

