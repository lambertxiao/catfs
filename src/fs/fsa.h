#include "fs/fuse.h"

namespace catfs {
  namespace fs {
    class FuseAdapter {
      public:
        static int statfs(const char* path, struct statvfs* stbuf);
        static int getattr(const char* path, struct stat* stbuf);
        static int readlink(const char* path, char* buf, size_t size);
        static int mknod(const char* path, mode_t mode, dev_t rdev);
        static int mkdir(const char* path, mode_t mode);
        static int unlink(const char* path);
        static int rmdir(const char* path);
        static int symlink(const char* from, const char* to);
        static int rename(const char* from, const char* to);
        static int link(const char* from, const char* to);
        static int chmod(const char* path, mode_t mode);
        static int chmod_nocopy(const char* path, mode_t mode);
        static int chown(const char* path, uid_t uid, gid_t gid);
        static int chown_nocopy(const char* path, uid_t uid, gid_t gid);
        static int utimens(const char* path, const struct timespec ts[2]);
        static int utimens_nocopy(const char* path, const struct timespec ts[2]);
        static int truncate(const char* path, off_t size);
        static int create(const char* path, mode_t mode, struct fuse_file_info* fi);
        static int open(const char* path, struct fuse_file_info* fi);
        static int read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi);
        static int write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi);
        static int flush(const char* path, struct fuse_file_info* fi);
        static int fsync(const char* path, int datasync, struct fuse_file_info* fi);
        static int release(const char* path, struct fuse_file_info* fi);
        static int opendir(const char* path, struct fuse_file_info* fi);
        static int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi);
        static int access(const char* path, int mask);
        static void * init(struct fuse_conn_info* conn);
        static void destroy(void*);
    };
  };
};
