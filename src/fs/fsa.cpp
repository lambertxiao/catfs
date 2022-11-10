#include "fmtlog/fmtlog.h"
#include <fmt/core.h>
#include <functional>
#include <iostream>

#include "fs/fs.h"
#include "fs/fsa.h"
#include "fs/fuse.h"
#include "types/inode.h"

namespace catfs {
namespace fs {
static CatFS *catfs;

void FuseAdapter::init_catfs(CatFS *cfs) { catfs = cfs; }

CatFS *FuseAdapter::get_catfs() { return catfs; }

void FuseAdapter::statfs(fuse_req_t req, fuse_ino_t ino) {
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

void FuseAdapter::init(void *userdata, struct fuse_conn_info *conn) {
  logi("fsa-init");
}

void FuseAdapter::destroy(void *userdata) {
  logi("fsa-destroy");
  free(catfs);
}

void FuseAdapter::lookup(fuse_req_t req, fuse_ino_t parent, const char *name) {
  logi("fsa-lookup pino:{} name:{}", parent, name);

  try {
    auto dentry = catfs->find_dentry(parent, name);
    if (dentry == NULL) {
      fuse_reply_err(req, ENOENT);
      return;
    }

    reply_inode(req, *dentry->inode);
    return;
  } catch (std::exception &e) {
    loge("find_dentry error, pino:{} name:{} err:{}", parent, name, e.what());
    fuse_reply_err(req, EIO);
    return;
  }
  logi("fsa-lookup-done pino:{} name:{}", parent, name);
}

void FuseAdapter::forget(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup) {
  logi("fsa-forget ino:{}", ino);
}

void FuseAdapter::getattr(fuse_req_t req, fuse_ino_t ino,
                          struct fuse_file_info *fi) {
  logi("fsa-getattr ino:{}", ino);
  try {
    auto inode = catfs->lookup_inode(ino);
    if (inode == NULL) {
      fuse_reply_err(req, ENOENT);
      return;
    }

    struct stat stbuf;
    memset(&stbuf, 0, sizeof(stbuf));
    fill_inode_attr(stbuf, *inode);
    fuse_reply_attr(req, &stbuf, 1.0);
  } catch (std::exception &e) {
    loge("lookup_inode error, ino:{} err:{}", ino, e.what());
    fuse_reply_err(req, EIO);
    return;
  }
  logi("fsa-getattr-done ino:{}", ino);
}

void FuseAdapter::fill_inode_attr(struct stat &stbuf,
                                  const types::Inode &inode) {
  stbuf.st_ino = inode.ino;
  stbuf.st_mode = inode.mode;
  stbuf.st_gid = inode.gid;
  stbuf.st_uid = inode.uid;
  stbuf.st_size = inode.size;
  stbuf.st_ctim = inode.ctime;
  stbuf.st_mtim = inode.mtime;
}

void FuseAdapter::setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr,
                          int to_set, struct fuse_file_info *fi) {
  logi("fsa-setattr ino:{} mode:{} to_set:{}", ino, attr->st_mode, to_set);
  fuse_reply_attr(req, attr, 1.0);
}

void FuseAdapter::readlink(fuse_req_t req, fuse_ino_t ino) {
  std::cout << "fsa-readlink" << std::endl;
  fuse_reply_err(req, EPERM);
}

void FuseAdapter::mknod(fuse_req_t req, fuse_ino_t parent, const char *name,
                        mode_t mode, dev_t rdev) {
  std::cout << "fsa-mknod" << std::endl;
}

void FuseAdapter::mkdir(fuse_req_t req, fuse_ino_t parent, const char *name,
                        mode_t mode) {
  logi("fsa-mkdir pino:{} name:{} mode:{}", parent, name, mode);
  try {
    mode = mode | S_IFDIR;
    auto dentry = catfs->create_dentry(parent, name, mode);
    if (dentry == NULL) {
      fuse_reply_err(req, EIO);
      return;
    }

    reply_inode(req, *dentry->inode);
    return;
  } catch (std::exception &e) {
    loge("fsa-mkdir error, pino:{} name:{} err:{}", parent, name, e.what());
    fuse_reply_err(req, EIO);
    return;
  }
}

void FuseAdapter::reply_inode(fuse_req_t &req, const Inode &inode) {
  struct fuse_entry_param e;
  memset(&e, 0, sizeof(e));
  e.ino = inode.ino;
  e.attr_timeout = 1.0;
  e.entry_timeout = 1.0;
  fill_inode_attr(e.attr, inode);
  fuse_reply_entry(req, &e);
}

void FuseAdapter::unlink(fuse_req_t req, fuse_ino_t parent, const char *name) {
  logi("fsa-unlink pino:{} name:{}", parent, name);
  try {
    catfs->remove_dentry(parent, name);
    fuse_reply_err(req, 0);
  } catch (std::exception &e) {
    loge("unlink error, pino:{} name:{} err:{}", parent, name, e.what());
    fuse_reply_err(req, EIO);
  }
}

void FuseAdapter::rmdir(fuse_req_t req, fuse_ino_t parent, const char *name) {
  logi("fsa-rmdir pino:{} name:{}", parent, name);
  try {
    catfs->remove_dentry(parent, name);
  } catch (types::ERR_ENOTEMPTY &e) {
    loge("rmdir error, pino:{} name:{} err:{}", parent, name, e.what());
    fuse_reply_err(req, ENOTEMPTY);
  } catch (std::exception &e) {
    loge("rmdir error, pino:{} name:{} err:{}", parent, name, e.what());
    fuse_reply_err(req, EIO);
  }
}

void FuseAdapter::symlink(fuse_req_t req, const char *link, fuse_ino_t parent,
                          const char *name) {
  std::cout << "fsa-symlink" << std::endl;
}

void FuseAdapter::rename(fuse_req_t req, fuse_ino_t parent, const char *name,
                         fuse_ino_t newparent, const char *newname,
                         unsigned int flags) {
  std::cout << "fsa-rename" << std::endl;
}
void FuseAdapter::link(fuse_req_t req, fuse_ino_t ino, fuse_ino_t newparent,
                       const char *newname) {
  std::cout << "fsa-link" << std::endl;
}
void FuseAdapter::open(fuse_req_t req, fuse_ino_t ino,
                       struct fuse_file_info *fi) {
  logi("fsa-open ino:{}", ino);

  try {
    auto hno = catfs->openfile(ino, fi->flags);
    fi->fh = hno;
    fuse_reply_open(req, fi);
    logi("fsa-open-done ino:{} hno:{}", ino, hno);
  } catch (types::ERR_ENOENT &e) {
    loge("openfile error, ino:{} err:{}", ino, e.what());
    fuse_reply_err(req, ENOENT);
  } catch (std::exception &e) {
    loge("openfile error, ino:{} err:{}", ino, e.what());
    fuse_reply_err(req, EIO);
  }
}

void FuseAdapter::create(fuse_req_t req, fuse_ino_t parent, const char *name,
                         mode_t mode, struct fuse_file_info *fi) {
  logi("fsa-create pino:{} name:{} mode:{}", parent, name, mode);
  try {
    auto dentry = catfs->create_dentry(parent, name, mode);
    if (dentry == NULL) {
      fuse_reply_err(req, EIO);
      return;
    }

    struct fuse_entry_param e;
    fill_fuse_entry_param(e, *dentry->inode);
    fuse_reply_create(req, &e, fi);
    return;
  } catch (std::exception &e) {
    loge("fsa-create error, pino:{} name:{} err:{}", parent, name, e.what());
    fuse_reply_err(req, EIO);
    return;
  }
}

void FuseAdapter::read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                       struct fuse_file_info *fi) {
  logi("fsa-read ino:{} off:{} size:{}", ino, off, size);

  try {
    char *buf = (char *)malloc(size * sizeof(char));
    int read_bytes = catfs->readfile(fi->fh, off, size, buf);
    fuse_reply_buf(req, buf, read_bytes);
    free(buf);
  } catch (const std::exception &e) {
    loge("fsa-read error, ino:{} off:{} size:{} err:{}", ino, off, size,
         e.what());
    fuse_reply_err(req, EIO);
    return;
  }
}

void FuseAdapter::write(fuse_req_t req, fuse_ino_t ino, const char *buf,
                        size_t size, off_t off, struct fuse_file_info *fi) {
  logi("fsa-write ino:{} off:{} size:{}", ino, off, size);
}

void FuseAdapter::flush(fuse_req_t req, fuse_ino_t ino,
                        struct fuse_file_info *fi) {
  logi("fsa-flush ino:{}", ino);
  fuse_reply_err(req, 0);
}

void FuseAdapter::release(fuse_req_t req, fuse_ino_t ino,
                          struct fuse_file_info *fi) {
  logi("fsa-release ino:{}", ino);
  fuse_reply_err(req, 0);
}

void FuseAdapter::fsync(fuse_req_t req, fuse_ino_t ino, int datasync,
                        struct fuse_file_info *fi) {
  std::cout << "fsa-fsync" << std::endl;
}

void FuseAdapter::opendir(fuse_req_t req, fuse_ino_t ino,
                          struct fuse_file_info *fi) {
  logi("fsa-opendir ino:{}", ino);

  try {
    auto hno = catfs->opendir(ino);
    fi->fh = hno;
    fuse_reply_open(req, fi);
    logi("fsa-opendir-done ino:{} hno:{}", ino, hno);
  } catch (types::ERR_ENOENT &e) {
    loge("opendir error, ino:{} err:{}", ino, e.what());
    fuse_reply_err(req, ENOENT);
  } catch (std::exception &e) {
    loge("opendir error, ino:{} err:{}", ino, e.what());
    fuse_reply_err(req, EIO);
  }
}

void FuseAdapter::readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
                          off_t off, struct fuse_file_info *fi) {
  logi("fsa-readdir ino:{} hno:{} offset:{} limit:{}", ino, fi->fh, off, size);

  try {
    auto dirents = catfs->read_dir(fi->fh, off, size);
    char *p;
    char *buf;

    size_t rem = size;
    int idx = 0;
    buf = (char *)calloc(1, size);
    p = buf;

    for (auto &d : dirents) {
      if (d.name == "." || d.name == "..")
        continue;

      struct stat st = {
          .st_ino = d.inode->ino,
          .st_mode = d.inode->mode,
      };
      size_t entsize =
          fuse_add_direntry(req, p, rem, d.name.c_str(), &st, off + idx);
      idx++;
      p += entsize;
      rem -= entsize;
    }
    fuse_reply_buf(req, buf, size - rem);
    free(buf);
  } catch (std::exception &e) {
    loge("readdir error, ino:{} offset:{} limit:{} err:{}", ino, off, size,
         e.what());
    fuse_reply_err(req, EIO);
  }
  logi("fsa-readdir-done ino:{} hno:{}", ino, fi->fh);
}

void FuseAdapter::readdirplus(fuse_req_t req, fuse_ino_t ino, size_t size,
                              off_t off, struct fuse_file_info *fi) {
  logi("fsa-readdirplus ino:{} hno:{} offset:{} limit:{}", ino, fi->fh, off,
       size);
  try {
    auto dirents = catfs->read_dir(fi->fh, off, size);

    char *p;
    char *buf;
    size_t rem = size;
    int idx = 0;
    buf = (char *)calloc(1, size);
    p = buf;

    for (auto &d : dirents) {
      struct fuse_entry_param e;
      fill_fuse_entry_param(e, *d.inode);
      size_t entsize =
          fuse_add_direntry_plus(req, p, rem, d.name.c_str(), &e, off + idx);
      idx++;

      p += entsize;
      rem -= entsize;
    }

    fuse_reply_buf(req, buf, size - rem);
    free(buf);
  } catch (std::exception &e) {
    loge("readdirplus error, ino:{} offset:{} limit:{} err:{}", ino, off, size,
         e.what());
    fuse_reply_err(req, EIO);
  }

  logi("fsa-readdirplus-done ino:{} hno:{}", ino, fi->fh);
}

void FuseAdapter::fill_fuse_entry_param(fuse_entry_param &e, Inode &inode) {
  memset(&e, 0, sizeof(e));
  e.ino = inode.ino;
  e.attr_timeout = 1.0;
  e.entry_timeout = 1.0;
  fill_inode_attr(e.attr, inode);
}

void FuseAdapter::releasedir(fuse_req_t req, fuse_ino_t ino,
                             struct fuse_file_info *fi) {
  logi("fsa-releasedir ino:{} hno:{}", ino, fi->fh);
  catfs->release_dir(ino, fi->fh);
}

void FuseAdapter::fsyncdir(fuse_req_t req, fuse_ino_t ino, int datasync,
                           struct fuse_file_info *fi) {
  std::cout << "fsa-fsyncdir" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::setxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
                           const char *value, size_t size, int flags) {
  std::cout << "fsa-setxattr" << std::endl;
  fuse_reply_err(req, ENOSYS);
}

void FuseAdapter::getxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
                           size_t size) {
  logi("fsa-getxattr pino:{} name:{}", ino, name);
  fuse_reply_err(req, ENOSYS);
}

void FuseAdapter::listxattr(fuse_req_t req, fuse_ino_t ino, size_t size) {
  std::cout << "fsa-listxattr" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::removexattr(fuse_req_t req, fuse_ino_t ino,
                              const char *name) {
  std::cout << "fsa-removexattr" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::access(fuse_req_t req, fuse_ino_t ino, int mask) {
  logi("fsa-access ino:{} mask:{}", ino, mask);
  fuse_reply_err(req, ENOSYS);
}

void FuseAdapter::getlk(fuse_req_t req, fuse_ino_t ino,
                        struct fuse_file_info *fi, struct flock *lock) {
  std::cout << "fsa-getlk" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::setlk(fuse_req_t req, fuse_ino_t ino,
                        struct fuse_file_info *fi, struct flock *lock,
                        int sleep) {
  std::cout << "fsa-setlk" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::bmap(fuse_req_t req, fuse_ino_t ino, size_t blocksize,
                       uint64_t idx) {
  std::cout << "fsa-bmap" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::poll(fuse_req_t req, fuse_ino_t ino,
                       struct fuse_file_info *fi, struct fuse_pollhandle *ph) {
  std::cout << "fsa-poll" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::write_buf(fuse_req_t req, fuse_ino_t ino,
                            struct fuse_bufvec *bufv, off_t off,
                            struct fuse_file_info *fi) {
  std::cout << "fsa-write_buf" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::retrieve_reply(fuse_req_t req, void *cookie, fuse_ino_t ino,
                                 off_t offset, struct fuse_bufvec *bufv) {
  std::cout << "fsa-retrieve_reply" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::forget_multi(fuse_req_t req, size_t count,
                               struct fuse_forget_data *forgets) {
  std::cout << "fsa-forget_multi" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::flock(fuse_req_t req, fuse_ino_t ino,
                        struct fuse_file_info *fi, int op) {
  std::cout << "fsa-flock" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::fallocate(fuse_req_t req, fuse_ino_t ino, int mode,
                            off_t offset, off_t length,
                            struct fuse_file_info *fi) {
  std::cout << "fsa-fallocate" << std::endl;
  fuse_reply_err(req, ENOSYS);
}

void FuseAdapter::copy_file_range(fuse_req_t req, fuse_ino_t ino_in,
                                  off_t off_in, struct fuse_file_info *fi_in,
                                  fuse_ino_t ino_out, off_t off_out,
                                  struct fuse_file_info *fi_out, size_t len,
                                  int flags) {
  std::cout << "fsa-copy_file_range" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
void FuseAdapter::lseek(fuse_req_t req, fuse_ino_t ino, off_t off, int whence,
                        struct fuse_file_info *fi) {
  std::cout << "fsa-lseek" << std::endl;
  fuse_reply_err(req, ENOSYS);
}
}; // namespace fs
}; // namespace catfs
