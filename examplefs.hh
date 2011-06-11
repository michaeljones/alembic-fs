
#ifndef examples_hh
#define examples_hh

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

class ExampleFS {
private: 
	const char *_root;

	static ExampleFS *_instance;

	void AbsPath(char dest[PATH_MAX], const char *path);

public:
	static ExampleFS *Instance();

	ExampleFS();
	~ExampleFS();

	void setRootDir(const char *path);

	int Getattr(const char *path, struct stat *statbuf);
	int Readlink(const char *path, char *link, size_t size);
	int Mknod(const char *path, mode_t mode, dev_t dev);
	int Mkdir(const char *path, mode_t mode);
	int Unlink(const char *path);
	int Rmdir(const char *path);
	int Symlink(const char *path, const char *link);
	int Rename(const char *path, const char *newpath);
	int Link(const char *path, const char *newpath);
	int Chmod(const char *path, mode_t mode);
	int Chown(const char *path, uid_t uid, gid_t gid);
	int Truncate(const char *path, off_t newSize);
	int Utime(const char *path, struct utimbuf *ubuf);
	int Open(const char *path, struct fuse_file_info *fileInfo);
	int Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
	int Write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
	int Statfs(const char *path, struct statvfs *statInfo);
	int Flush(const char *path, struct fuse_file_info *fileInfo);
	int Release(const char *path, struct fuse_file_info *fileInfo);
	int Fsync(const char *path, int datasync, struct fuse_file_info *fi);
	int Setxattr(const char *path, const char *name, const char *value, size_t size, int flags);
	int Getxattr(const char *path, const char *name, char *value, size_t size);
	int Listxattr(const char *path, char *list, size_t size);
	int Removexattr(const char *path, const char *name);
	int Opendir(const char *path, struct fuse_file_info *fileInfo);
	int Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo);
	int Releasedir(const char *path, struct fuse_file_info *fileInfo);
	int Fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo);
	int Init(struct fuse_conn_info *conn);
	int Truncate(const char *path, off_t offset, struct fuse_file_info *fileInfo);
};



#endif //examples_hh
