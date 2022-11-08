#ifndef CATFS_FS_FSA_H_
#define CATFS_FS_FSA_H_

#include <fmt/core.h>
#include <functional>
#include "fmtlog/fmtlog.h"
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

    static CatFS *catfs;

    class FuseAdapter
    {
    public:
      static void init_catfs(CatFS *cfs)
      {
        catfs = cfs;
      }

      static CatFS *get_catfs()
      {
        return catfs;
      }

      static void statfs(fuse_req_t req, fuse_ino_t ino)
      {
        logi("fsa-statfs inode:{}", ino);

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

      static void init(void *userdata, struct fuse_conn_info *conn)
      {
        logi("fsa-init");
      }

      static void destroy(void *userdata)
      {
        logi("fsa-destroy");
        free(catfs);
      }

      static void lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
      {
        logi("fsa-lookup pino:{} name:{}", parent, name);

        try
        {
          auto dentry = catfs->find_dentry(parent, name);
          if (dentry == NULL)
          {
            fuse_reply_err(req, ENOENT);
            return;
          }

          reply_dentry(req, dentry);
          return;
        }
        catch (std::exception &e)
        {
          loge("find_dentry error, pino:{} name:{} err:{}", parent, name, e.what());
          fuse_reply_err(req, EIO);
          return;
        }
        logi("fsa-lookup-done pino:{} name:{}", parent, name);
      }

      static void forget(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup)
      {
        logi("fsa-forget ino:{}", ino);
      }

      static void getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
      {
        logi("fsa-getattr ino:{}", ino);
        try
        {
          auto inode = catfs->lookup_inode(ino);
          if (inode == NULL)
          {
            fuse_reply_err(req, ENOENT);
            return;
          }

          struct stat stbuf;
          memset(&stbuf, 0, sizeof(stbuf));
          fill_inode_attr(stbuf, inode);
          fuse_reply_attr(req, &stbuf, 1.0);
        }
        catch (std::exception &e)
        {
          loge("lookup_inode error, ino:{} err:{}", ino, e.what());
          fuse_reply_err(req, EIO);
          return;
        }
        logi("fsa-getattr-done ino:{}", ino);
      }

      static void fill_inode_attr(struct stat &stbuf, const types::Inode *inode)
      {
        stbuf.st_ino = inode->ino;
        stbuf.st_mode = inode->mode;
        stbuf.st_gid = inode->gid;
        stbuf.st_uid = inode->uid;
        stbuf.st_size = inode->size;
        stbuf.st_ctim = inode->ctime;
        stbuf.st_mtim = inode->mtime;
      }

      static void setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr, int to_set, struct fuse_file_info *fi)
      {
        std::cout << "fsa-setattr" << std::endl;
        fuse_reply_err(req, EPERM);
      }

      static void readlink(fuse_req_t req, fuse_ino_t ino)
      {
        std::cout << "fsa-readlink" << std::endl;
        fuse_reply_err(req, EPERM);
      }

      static void mknod(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode, dev_t rdev)
      {
        std::cout << "fsa-mknod" << std::endl;
      }

      static void mkdir(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode)
      {
        logi("fsa-mkdir pino:{} name:{} mode:{}", parent, name, mode);
        try
        {
          mode = mode | S_IFDIR;
          auto dentry = catfs->create_dentry(parent, name, mode);
          if (dentry == NULL)
          {
            fuse_reply_err(req, EIO);
            return;
          }

          reply_dentry(req, dentry);
          return;
        }
        catch (std::exception &e)
        {
          loge("fsa-mkdir error, pino:{} name:{} err:{}", parent, name, e.what());
          fuse_reply_err(req, EIO);
          return;
        }
      }

      static void reply_dentry(fuse_req_t &req, const Dentry *dentry)
      {
        struct fuse_entry_param e;
        memset(&e, 0, sizeof(e));
        e.ino = dentry->inode->ino;
        e.attr_timeout = 1.0;
        e.entry_timeout = 1.0;
        fill_inode_attr(e.attr, dentry->inode);
        fuse_reply_entry(req, &e);
      }

      static void unlink(fuse_req_t req, fuse_ino_t parent, const char *name)
      {
        logi("fsa-unlink pino:{} name:{}", parent, name);
        try
        {
          catfs->remove_dentry(parent, name);
        }
        catch (std::exception &e)
        {
          loge("unlink error, pino:{} name:{} err:{}", parent, name, e.what());
          fuse_reply_err(req, EIO);
        }
      }

      static void rmdir(fuse_req_t req, fuse_ino_t parent, const char *name)
      {
        logi("fsa-rmdir pino:{} name:{}", parent, name);
        try
        {
          catfs->remove_dentry(parent, name);
        }
        catch (types::ERR_ENOTEMPTY &e)
        {
          loge("rmdir error, pino:{} name:{} err:{}", parent, name, e.what());
          fuse_reply_err(req, ENOTEMPTY);
        }
        catch (std::exception &e)
        {
          loge("rmdir error, pino:{} name:{} err:{}", parent, name, e.what());
          fuse_reply_err(req, EIO);
        }
      }

      static void symlink(fuse_req_t req, const char *link, fuse_ino_t parent, const char *name)
      {
        std::cout << "fsa-symlink" << std::endl;
      }

      static void rename(fuse_req_t req, fuse_ino_t parent, const char *name, fuse_ino_t newparent, const char *newname, unsigned int flags)
      {
        std::cout << "fsa-rename" << std::endl;
      }
      static void link(fuse_req_t req, fuse_ino_t ino, fuse_ino_t newparent, const char *newname)
      {
        std::cout << "fsa-link" << std::endl;
      }
      static void open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
      {
        logi("fsa-open ino:{}", ino);

        try
        {
          auto hno = catfs->openfile(ino);
          fi->fh = hno;
          fuse_reply_open(req, fi);
          logi("fsa-open-done ino:{} hno:{}", ino, hno);
        }
        catch (types::ERR_ENOENT &e)
        {
          loge("openfile error, ino:{} err:{}", ino, e.what());
          fuse_reply_err(req, ENOENT);
        }
        catch (std::exception &e)
        {
          loge("openfile error, ino:{} err:{}", ino, e.what());
          fuse_reply_err(req, EIO);
        }
      }

      static void create(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode, struct fuse_file_info *fi)
      {
        logi("fsa-create pino:{} name:{} mode:{}", parent, name, mode);
        try
        {
          auto dentry = catfs->create_dentry(parent, name, mode);
          if (dentry == NULL)
          {
            fuse_reply_err(req, EIO);
            return;
          }

          reply_dentry(req, dentry);
          return;
        }
        catch (std::exception &e)
        {
          loge("fsa-create error, pino:{} name:{} err:{}", parent, name, e.what());
          fuse_reply_err(req, EIO);
          return;
        }
      }

      static void read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi)
      {
        logi("fsa-read ino:{} off:{} size:{}", ino, off, size);
      }

      static void write(fuse_req_t req, fuse_ino_t ino, const char *buf, size_t size, off_t off, struct fuse_file_info *fi)
      {
        logi("fsa-write ino:{} off:{} size:{}", ino, off, size);
      }

      static void flush(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
      {
        logi("fsa-flush ino:{}", ino);
      }

      static void release(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
      {
        logi("fsa-release ino:{}", ino);
      }

      static void fsync(fuse_req_t req, fuse_ino_t ino, int datasync, struct fuse_file_info *fi)
      {
        std::cout << "fsa-fsync" << std::endl;
      }

      static void opendir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
      {
        logi("fsa-opendir ino:{}", ino);

        try
        {
          auto hno = catfs->opendir(ino);
          fi->fh = hno;
          fuse_reply_open(req, fi);
          logi("fsa-opendir-done ino:{} hno:{}", ino, hno);
        }
        catch (types::ERR_ENOENT &e)
        {
          loge("opendir error, ino:{} err:{}", ino, e.what());
          fuse_reply_err(req, ENOENT);
        }
        catch (std::exception &e)
        {
          loge("opendir error, ino:{} err:{}", ino, e.what());
          fuse_reply_err(req, EIO);
        }
      }

      static void readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi)
      {
        logi("fsa-readdir ino:{} hno:{} offset:{} limit:{}", ino, fi->fh, off, size);

        try
        {
          auto dirents = catfs->read_dir(fi->fh, off, size);
          char *p;
          char *buf;

          size_t rem = size;
          int idx = 0;
          buf = (char *)calloc(1, size);
          p = buf;

          for (auto &d : dirents)
          {
            if (d.name == "." || d.name == "..")
              continue;

            struct stat st = {
                .st_ino = d.inode->ino,
                .st_mode = d.inode->mode,
            };
            size_t entsize = fuse_add_direntry(req, p, rem, d.name.c_str(), &st, off + idx);
            idx++;
            p += entsize;
            rem -= entsize;
          }
          fuse_reply_buf(req, buf, size - rem);
          free(buf);
        }
        catch (std::exception &e)
        {
          loge("readdir error, ino:{} offset:{} limit:{} err:{}", ino, off, size, e.what());
          fuse_reply_err(req, EIO);
        }
        logi("fsa-readdir-done ino:{} hno:{}", ino, fi->fh);
      }

      static void readdirplus(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi)
      {
        logi("fsa-readdirplus ino:{} hno:{} offset:{} limit:{}", ino, fi->fh, off, size);
        try
        {
          auto dirents = catfs->read_dir(fi->fh, off, size);

          char *p;
          char *buf;
          size_t rem = size;
          int idx = 0;
          buf = (char *)calloc(1, size);
          p = buf;

          for (auto &d : dirents)
          {
            struct fuse_entry_param e;
            fill_fuse_entry_param(e, d);
            size_t entsize = fuse_add_direntry_plus(req, p, rem, d.name.c_str(), &e, off + idx);
            idx++;

            p += entsize;
            rem -= entsize;
          }

          fuse_reply_buf(req, buf, size - rem);
          free(buf);
        }
        catch (std::exception &e)
        {
          loge("readdirplus error, ino:{} offset:{} limit:{} err:{}", ino, off, size, e.what());
          fuse_reply_err(req, EIO);
        }

        logi("fsa-readdirplus-done ino:{} hno:{}", ino, fi->fh);
      }

      static void fill_fuse_entry_param(fuse_entry_param &e, Dirent &dirent)
      {
        memset(&e, 0, sizeof(e));
        e.ino = dirent.inode->ino;
        e.attr_timeout = 1.0;
        e.entry_timeout = 1.0;
        fill_inode_attr(e.attr, dirent.inode);
      }

      static void releasedir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
      {
        logi("fsa-releasedir ino:{} hno:{}", ino, fi->fh);
        catfs->release_dir(ino, fi->fh);
      }

      static void fsyncdir(fuse_req_t req, fuse_ino_t ino, int datasync, struct fuse_file_info *fi)
      {
        std::cout << "fsa-fsyncdir" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void setxattr(fuse_req_t req, fuse_ino_t ino, const char *name, const char *value, size_t size, int flags)
      {
        std::cout << "fsa-setxattr" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }

      static void getxattr(fuse_req_t req, fuse_ino_t ino, const char *name, size_t size)
      {
        logi("fsa-getxattr pino:{} name:{}", ino, name);
        fuse_reply_err(req, ENOSYS);
      }

      static void listxattr(fuse_req_t req, fuse_ino_t ino, size_t size)
      {
        std::cout << "fsa-listxattr" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void removexattr(fuse_req_t req, fuse_ino_t ino, const char *name)
      {
        std::cout << "fsa-removexattr" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void access(fuse_req_t req, fuse_ino_t ino, int mask)
      {
        logi("fsa-access ino:{} mask:{}", ino, mask);
        fuse_reply_err(req, ENOSYS);
      }

      static void getlk(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct flock *lock)
      {
        std::cout << "fsa-getlk" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void setlk(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct flock *lock, int sleep)
      {
        std::cout << "fsa-setlk" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void bmap(fuse_req_t req, fuse_ino_t ino, size_t blocksize, uint64_t idx)
      {
        std::cout << "fsa-bmap" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void poll(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, struct fuse_pollhandle *ph)
      {
        std::cout << "fsa-poll" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void write_buf(fuse_req_t req, fuse_ino_t ino, struct fuse_bufvec *bufv, off_t off, struct fuse_file_info *fi)
      {
        std::cout << "fsa-write_buf" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void retrieve_reply(fuse_req_t req, void *cookie, fuse_ino_t ino, off_t offset, struct fuse_bufvec *bufv)
      {
        std::cout << "fsa-retrieve_reply" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void forget_multi(fuse_req_t req, size_t count, struct fuse_forget_data *forgets)
      {
        std::cout << "fsa-forget_multi" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void flock(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi, int op)
      {
        std::cout << "fsa-flock" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void fallocate(fuse_req_t req, fuse_ino_t ino, int mode, off_t offset, off_t length, struct fuse_file_info *fi)
      {
        std::cout << "fsa-fallocate" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }

      static void copy_file_range(fuse_req_t req, fuse_ino_t ino_in, off_t off_in, struct fuse_file_info *fi_in, fuse_ino_t ino_out, off_t off_out, struct fuse_file_info *fi_out, size_t len, int flags)
      {
        std::cout << "fsa-copy_file_range" << std::endl;
        fuse_reply_err(req, ENOSYS);
      }
      static void lseek(fuse_req_t req, fuse_ino_t ino, off_t off, int whence, struct fuse_file_info *fi)
      {
        std::cout << "fsa-lseek" << std::endl;
        fuse_reply_err(req, ENOSYS);
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
