#ifndef CATFS_FS_FSA_H_
#define CATFS_FS_FSA_H_

#include <fmt/core.h>
#include "fs/fuse.h"
#include "types/inode.h"

static const uint32_t BLOCK_SIZE = 4096;
static const uint64_t TOTAL_SPACE = 1 << 30; // 1PB
static const uint32_t TOTAL_BLOCKS = TOTAL_SPACE / BLOCK_SIZE;
static const uint32_t INODES = 1 * 1000 * 1000 * 1000; // 1 billion

namespace catfs {
  namespace fs {

    static CatFS* catfs;

    class FuseAdapter {
      public:
        
        static void init_catfs(CatFS* cfs) {
          catfs = cfs;
        }

        static CatFS* get_catfs() {
          return catfs;
        }

        static void statfs(fuse_req_t req, fuse_ino_t ino) {
          fmt::print("statfs inode:{}\n", ino);

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
          std::cout << "fsa-init" << std::endl;
        }

        static void destroy(void *userdata) {
          std::cout << "fsa-destroy" << std::endl;
          free(catfs);
        }

        static void lookup(fuse_req_t req, fuse_ino_t parent, const char *name) {
          std::cout << "fsa-lookup" << std::endl;
        }
        static void forget(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup) {
          std::cout << "fsa-forget" << std::endl;
        }
        static void getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          fmt::print("fsa-getattr inode:{}\n", ino);
          // auto inode = CatFS::getInstance().lookupInode((catfs::types::InodeID)ino);

          struct stat stbuf;
          (void) fi;

          memset(&stbuf, 0, sizeof(stbuf));
          if (hello_stat(ino, &stbuf) == -1)
            fuse_reply_err(req, ENOENT);
          else
            fuse_reply_attr(req, &stbuf, 1.0);
        }

        static int hello_stat(fuse_ino_t ino, struct stat *stbuf)
        {
          stbuf->st_ino = ino;
          switch (ino) {
          case 1:
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            break;

          case 2:
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;
            stbuf->st_size = 1024;
            break;

          default:
            return -1;
          }
          return 0;
        }

        static void setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr, int to_set, struct fuse_file_info *fi) {
          std::cout << "fsa-setattr" << std::endl;
        }
        static void readlink(fuse_req_t req, fuse_ino_t ino) {
          std::cout << "fsa-readlink" << std::endl;
        }
        static void mknod(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode, dev_t rdev) {
          std::cout << "fsa-mknod" << std::endl;
        }
        static void mkdir(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode) {
          std::cout << "fsa-mkdir" << std::endl;
        }
        static void unlink(fuse_req_t req, fuse_ino_t parent, const char *name) {
          std::cout << "fsa-unlink" << std::endl;
        }
        static void rmdir(fuse_req_t req, fuse_ino_t parent, const char *name) {
          std::cout << "fsa-rmdir" << std::endl;
        }
        static void symlink(fuse_req_t req, const char *link, fuse_ino_t parent, const char *name) {
          std::cout << "fsa-symlink" << std::endl;
        }
        static void rename(fuse_req_t req, fuse_ino_t parent, const char *name, fuse_ino_t newparent, const char *newname, unsigned int flags) {
          std::cout << "fsa-rename" << std::endl;
        }
        static void link(fuse_req_t req, fuse_ino_t ino, fuse_ino_t newparent, const char *newname) {
          std::cout << "fsa-link" << std::endl;
        }
        static void open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          std::cout << "fsa-open" << std::endl;
        }
        static void read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi) {
          std::cout << "fsa-read" << std::endl;
        }
        static void write(fuse_req_t req, fuse_ino_t ino, const char *buf, size_t size, off_t off, struct fuse_file_info *fi) {
          std::cout << "fsa-write" << std::endl;
        }
        static void flush(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          std::cout << "fsa-flush" << std::endl;
        }
        static void release(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          std::cout << "fsa-release" << std::endl;
        }
        static void fsync(fuse_req_t req, fuse_ino_t ino, int datasync, struct fuse_file_info *fi) {
          std::cout << "fsa-fsync" << std::endl;
        }
        static void opendir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          std::cout << "fsa-opendir" << std::endl;
        }
        static void readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi) {
          std::cout << "fsa-readdir" << std::endl;
        }
        static void releasedir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
          std::cout << "fsa-releasedir" << std::endl;
        }
        static void fsyncdir(fuse_req_t req, fuse_ino_t ino, int datasync, struct fuse_file_info *fi) {
          std::cout << "fsa-fsyncdir" << std::endl;
        }
        static void setxattr(fuse_req_t req, fuse_ino_t ino, const char *name, const char *value, size_t size, int flags) {
          std::cout << "fsa-setxattr" << std::endl;
        }
        static void getxattr(fuse_req_t req, fuse_ino_t ino, const char *name, size_t size) {
          std::cout << "fsa-getxattr" << std::endl;
        }
        static void listxattr(fuse_req_t req, fuse_ino_t ino, size_t size) {
          std::cout << "fsa-listxattr" << std::endl;
        }
        static void removexattr(fuse_req_t req, fuse_ino_t ino, const char *name) {
          std::cout << "fsa-removexattr" << std::endl;
        }
        static void access(fuse_req_t req, fuse_ino_t ino, int mask) {
          std::cout << "fsa-access" << std::endl;
        }
        static void create(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode, struct fuse_file_info *fi) {
          std::cout << "fsa-create" << std::endl;
        }
        static void getlk(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct flock *lock) {
          std::cout << "fsa-getlk" << std::endl;
        }
        static void setlk(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct flock *lock, int sleep) {
          std::cout << "fsa-setlk" << std::endl;
        }
        static void bmap(fuse_req_t req, fuse_ino_t ino, size_t blocksize, uint64_t idx) {
          std::cout << "fsa-bmap" << std::endl;
        }
        static void poll(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct fuse_pollhandle *ph) {
          std::cout << "fsa-poll" << std::endl;
        }
        static void write_buf(fuse_req_t req, fuse_ino_t ino, struct fuse_bufvec *bufv, off_t off, struct fuse_file_info *fi) {
          std::cout << "fsa-write_buf" << std::endl;
        }
        static void retrieve_reply(fuse_req_t req, void *cookie, fuse_ino_t ino, off_t offset, struct fuse_bufvec *bufv) {
          std::cout << "fsa-retrieve_reply" << std::endl;
        }
        static void forget_multi(fuse_req_t req, size_t count, struct fuse_forget_data *forgets) {
          std::cout << "fsa-forget_multi" << std::endl;
        }
	      static void flock(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, int op) {
          std::cout << "fsa-flock" << std::endl;
        }
	      static void fallocate(fuse_req_t req, fuse_ino_t ino, int mode, off_t offset, off_t length, struct fuse_file_info *fi) {
          std::cout << "fsa-fallocate" << std::endl;
        }
	      static void readdirplus(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi) {
          std::cout << "fsa-readdirplus" << std::endl;
        }
	      static void copy_file_range(fuse_req_t req, fuse_ino_t ino_in, off_t off_in, struct fuse_file_info *fi_in, fuse_ino_t ino_out, off_t off_out, struct fuse_file_info *fi_out, size_t len, int flags) {
          std::cout << "fsa-copy_file_range" << std::endl;
        }
	      static void lseek(fuse_req_t req, fuse_ino_t ino, off_t off, int whence, struct fuse_file_info *fi) {
          std::cout << "fsa-lseek" << std::endl;
        }
    };

    static const struct fuse_lowlevel_ops catfs_oper = {
      .init = FuseAdapter::init,
      .destroy = FuseAdapter::destroy,
      .lookup = FuseAdapter::lookup,
      .forget = FuseAdapter::forget,
      .getattr = FuseAdapter::getattr,
      .setattr = FuseAdapter::setattr,
      .readlink = FuseAdapter::readlink,
      .mknod = FuseAdapter::mknod,
      .mkdir = FuseAdapter::mkdir,
      .unlink = FuseAdapter::unlink,
      .rmdir = FuseAdapter::rmdir,
      .symlink = FuseAdapter::symlink,
      .rename = FuseAdapter::rename,
      .link = FuseAdapter::link,
      .open = FuseAdapter::open,
      .read = FuseAdapter::read,
      .write = FuseAdapter::write,
      .flush = FuseAdapter::flush,
      .release = FuseAdapter::release,
      .fsync = FuseAdapter::fsync,
      .opendir = FuseAdapter::opendir,
      .readdir = FuseAdapter::readdir,
      .releasedir = FuseAdapter::releasedir,
      .fsyncdir = FuseAdapter::fsyncdir,
      .statfs	= FuseAdapter::statfs,
      .setxattr = FuseAdapter::setxattr,
      .getxattr = FuseAdapter::getxattr,
      .listxattr = FuseAdapter::listxattr,
      .removexattr = FuseAdapter::removexattr,
      .access = FuseAdapter::access,
      .create = FuseAdapter::create,
      .getlk = FuseAdapter::getlk,
      .setlk = FuseAdapter::setlk,
      .bmap = FuseAdapter::bmap,
      .poll = FuseAdapter::poll,
      .write_buf = FuseAdapter::write_buf,
      .retrieve_reply = FuseAdapter::retrieve_reply,
      .forget_multi = FuseAdapter::forget_multi,
      .flock = FuseAdapter::flock,
      .fallocate = FuseAdapter::fallocate,
      .readdirplus = FuseAdapter::readdirplus,
      .copy_file_range = FuseAdapter::copy_file_range,
      .lseek = FuseAdapter::lseek,
    };
  };
};
#endif
