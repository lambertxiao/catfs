#ifndef CATFS_FS_FSA_H_
#define CATFS_FS_FSA_H_

#include <fmt/core.h>
#include <functional>
#include "fmtlog/fmtlog.h"
#include "fs/fs.h"
#include "fs/fuse.h"
#include "types/inode.h"

static const uint32_t BLOCK_SIZE = 4096;
static const uint64_t TOTAL_SPACE = 1 << 30; // 1PB
static const uint32_t TOTAL_BLOCKS = TOTAL_SPACE / BLOCK_SIZE;
static const uint32_t INODES = 1 * 1000 * 1000 * 1000; // 1 billion

namespace catfs
{
  namespace fs
  {
    class FuseAdapter
    {
    public:
      static void init_catfs(CatFS *cfs);
      static CatFS *get_catfs();
      static void statfs(fuse_req_t req, fuse_ino_t ino);
      static void reply_inode(fuse_req_t &req, const Inode &inode);
      static void fill_fuse_entry_param(fuse_entry_param &e, Inode &inode);
      static void fill_inode_attr(struct stat &stbuf, const types::Inode &inode);

      static void init(void *userdata, struct fuse_conn_info *conn);
      static void destroy(void *userdata);
      static void lookup(fuse_req_t req, fuse_ino_t parent, const char *name);
      static void forget(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup);
      static void getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
      static void setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr, int to_set, struct fuse_file_info *fi);
      static void readlink(fuse_req_t req, fuse_ino_t ino);
      static void mknod(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode, dev_t rdev);
      static void mkdir(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode);
      static void unlink(fuse_req_t req, fuse_ino_t parent, const char *name);
      static void rmdir(fuse_req_t req, fuse_ino_t parent, const char *name);
      static void symlink(fuse_req_t req, const char *link, fuse_ino_t parent, const char *name);
      static void rename(fuse_req_t req, fuse_ino_t parent, const char *name, fuse_ino_t newparent, const char *newname, unsigned int flags);
      static void link(fuse_req_t req, fuse_ino_t ino, fuse_ino_t newparent, const char *newname);
      
      static void open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
      static void create(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode, struct fuse_file_info *fi);
      static void read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi);
      static void write(fuse_req_t req, fuse_ino_t ino, const char *buf, size_t size, off_t off, struct fuse_file_info *fi);
      static void flush(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
      static void release(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
      
      static void fsync(fuse_req_t req, fuse_ino_t ino, int datasync, struct fuse_file_info *fi);
      static void opendir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
      static void readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi);
      static void readdirplus(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi);
      static void releasedir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);
      static void fsyncdir(fuse_req_t req, fuse_ino_t ino, int datasync, struct fuse_file_info *fi);
      static void setxattr(fuse_req_t req, fuse_ino_t ino, const char *name, const char *value, size_t size, int flags);
      static void getxattr(fuse_req_t req, fuse_ino_t ino, const char *name, size_t size);
      static void listxattr(fuse_req_t req, fuse_ino_t ino, size_t size);
      static void removexattr(fuse_req_t req, fuse_ino_t ino, const char *name);
      static void access(fuse_req_t req, fuse_ino_t ino, int mask);
      static void getlk(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct flock *lock);
      static void setlk(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct flock *lock, int sleep);
      static void bmap(fuse_req_t req, fuse_ino_t ino, size_t blocksize, uint64_t idx);
      static void poll(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct fuse_pollhandle *ph);
      static void write_buf(fuse_req_t req, fuse_ino_t ino, struct fuse_bufvec *bufv, off_t off, struct fuse_file_info *fi);
      static void retrieve_reply(fuse_req_t req, void *cookie, fuse_ino_t ino, off_t offset, struct fuse_bufvec *bufv);
      static void forget_multi(fuse_req_t req, size_t count, struct fuse_forget_data *forgets);
      static void flock(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, int op);
      static void fallocate(fuse_req_t req, fuse_ino_t ino, int mode, off_t offset, off_t length, struct fuse_file_info *fi);
      static void copy_file_range(fuse_req_t req, fuse_ino_t ino_in, off_t off_in, struct fuse_file_info *fi_in, fuse_ino_t ino_out, off_t off_out, struct fuse_file_info *fi_out, size_t len, int flags);
      static void lseek(fuse_req_t req, fuse_ino_t ino, off_t off, int whence, struct fuse_file_info *fi);
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
        .statfs = FuseAdapter::statfs,
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
