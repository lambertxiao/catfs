#include "fs/fuse.h"

namespace catfs {
  namespace fs {
    class FuseAdapter {
      public:
        static const uint32_t BLOCK_SIZE = 4096;
        static const uint64_t TOTAL_SPACE = 1 << 30; // 1PB
	      static const uint32_t TOTAL_BLOCKS = TOTAL_SPACE / BLOCK_SIZE;
      	static const uint32_t INODES = 1 * 1000 * 1000 * 1000; // 1 billion

        static void statfs(fuse_req_t req, fuse_ino_t ino) {
          std::cout << "fuseop-statfs" << std::endl;

          struct statvfs stbuf;
          stbuf.f_bsize = BLOCK_SIZE;
          stbuf.f_frsize = BLOCK_SIZE;
          stbuf.f_blocks = TOTAL_BLOCKS;
          stbuf.f_bfree = TOTAL_BLOCKS;
          stbuf.f_bavail = TOTAL_BLOCKS;
          stbuf.f_files = INODES;
          stbuf.f_ffree = INODES;
          stbuf.f_favail = INODES;
          fuse_reply_statfs(req, &stbuf);
        }

        static void init(void *userdata, struct fuse_conn_info *conn) {
          std::cout << "fuseop-init" << std::endl;
        }
        static void destroy(void *userdata) {
          std::cout << "fuseop-destroy" << std::endl;
        }
        static void lookup(fuse_req_t req, fuse_ino_t parent, const char *name) {
          std::cout << "fuseop-lookup" << std::endl;
        }
        static void forget(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup) {
          std::cout << "fuseop-forget" << std::endl;
        }
        static void getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          std::cout << "fuseop-getattr" << std::endl;
        }
        static void setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr, int to_set, struct fuse_file_info *fi) {
          std::cout << "fuseop-setattr" << std::endl;
        }
        static void readlink(fuse_req_t req, fuse_ino_t ino) {
          std::cout << "fuseop-readlink" << std::endl;
        }
        static void mknod(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode, dev_t rdev) {
          std::cout << "fuseop-mknod" << std::endl;
        }
        static void mkdir(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode) {
          std::cout << "fuseop-mkdir" << std::endl;
        }
        static void unlink(fuse_req_t req, fuse_ino_t parent, const char *name) {
          std::cout << "fuseop-unlink" << std::endl;
        }
        static void rmdir(fuse_req_t req, fuse_ino_t parent, const char *name) {
          std::cout << "fuseop-rmdir" << std::endl;
        }
        static void symlink(fuse_req_t req, const char *link, fuse_ino_t parent, const char *name) {
          std::cout << "fuseop-symlink" << std::endl;
        }
        static void rename(fuse_req_t req, fuse_ino_t parent, const char *name, fuse_ino_t newparent, const char *newname, unsigned int flags) {
          std::cout << "fuseop-rename" << std::endl;
        }
        static void link(fuse_req_t req, fuse_ino_t ino, fuse_ino_t newparent, const char *newname) {
          std::cout << "fuseop-link" << std::endl;
        }
        static void open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          std::cout << "fuseop-open" << std::endl;
        }
        static void read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi) {
          std::cout << "fuseop-read" << std::endl;
        }
        static void write(fuse_req_t req, fuse_ino_t ino, const char *buf, size_t size, off_t off, struct fuse_file_info *fi) {
          std::cout << "fuseop-write" << std::endl;
        }
        static void flush(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          std::cout << "fuseop-flush" << std::endl;
        }
        static void release(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          std::cout << "fuseop-release" << std::endl;
        }
        static void fsync(fuse_req_t req, fuse_ino_t ino, int datasync, struct fuse_file_info *fi) {
          std::cout << "fuseop-fsync" << std::endl;
        }
        static void opendir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          std::cout << "fuseop-opendir" << std::endl;
        }
        static void readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi) {
          std::cout << "fuseop-readdir" << std::endl;
        }
        static void releasedir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          std::cout << "fuseop-releasedir" << std::endl;
        }
        static void fsyncdir(fuse_req_t req, fuse_ino_t ino, int datasync, struct fuse_file_info *fi) {
          std::cout << "fuseop-fsyncdir" << std::endl;
        }
        static void setxattr(fuse_req_t req, fuse_ino_t ino, const char *name, const char *value, size_t size, int flags) {
          std::cout << "fuseop-setxattr" << std::endl;
        }
        static void getxattr(fuse_req_t req, fuse_ino_t ino, const char *name, size_t size) {
          std::cout << "fuseop-getxattr" << std::endl;
        }
        static void listxattr(fuse_req_t req, fuse_ino_t ino, size_t size) {
          std::cout << "fuseop-listxattr" << std::endl;
        }
        static void removexattr(fuse_req_t req, fuse_ino_t ino, const char *name) {
          std::cout << "fuseop-removexattr" << std::endl;
        }
        static void access(fuse_req_t req, fuse_ino_t ino, int mask) {
          std::cout << "fuseop-access" << std::endl;
        }
        static void create(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode, struct fuse_file_info *fi) {
          std::cout << "fuseop-create" << std::endl;
        }
        static void getlk(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct flock *lock) {
          std::cout << "fuseop-getlk" << std::endl;
        }
        static void setlk(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct flock *lock, int sleep) {
          std::cout << "fuseop-setlk" << std::endl;
        }
        static void bmap(fuse_req_t req, fuse_ino_t ino, size_t blocksize, uint64_t idx) {
          std::cout << "fuseop-bmap" << std::endl;
        }
        static void poll(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct fuse_pollhandle *ph) {
          std::cout << "fuseop-poll" << std::endl;
        }
        static void write_buf(fuse_req_t req, fuse_ino_t ino, struct fuse_bufvec *bufv, off_t off, struct fuse_file_info *fi) {
          std::cout << "fuseop-write_buf" << std::endl;
        }
        static void retrieve_reply(fuse_req_t req, void *cookie, fuse_ino_t ino, off_t offset, struct fuse_bufvec *bufv) {
          std::cout << "fuseop-retrieve_reply" << std::endl;
        }
        static void forget_multi(fuse_req_t req, size_t count, struct fuse_forget_data *forgets) {
          std::cout << "fuseop-forget_multi" << std::endl;
        }
	      static void flock(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, int op) {
          std::cout << "fuseop-flock" << std::endl;
        }
	      static void fallocate(fuse_req_t req, fuse_ino_t ino, int mode, off_t offset, off_t length, struct fuse_file_info *fi) {
          std::cout << "fuseop-fallocate" << std::endl;
        }
	      static void readdirplus(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi) {
          std::cout << "fuseop-readdirplus" << std::endl;
        }
	      static void copy_file_range(fuse_req_t req, fuse_ino_t ino_in, off_t off_in, struct fuse_file_info *fi_in, fuse_ino_t ino_out, off_t off_out, struct fuse_file_info *fi_out, size_t len, int flags) {
          std::cout << "fuseop-copy_file_range" << std::endl;
        }
	      static void lseek(fuse_req_t req, fuse_ino_t ino, off_t off, int whence, struct fuse_file_info *fi) {
          std::cout << "fuseop-lseek" << std::endl;
        }
    };
  };
};
	