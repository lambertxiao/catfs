#ifndef CATFS_META_H
#define CATFS_META_H

#include "types/inode.h"
#include "types/dentry.h"
#include "types/dirent.h"
#include "types/errors.h"

#include <string>

namespace catfs {
  namespace meta {
    class Meta {
    public:
      // virtual types::Dirent *create_dentry(types::InodeID pino, std::string name, types::FileMode mode) = 0;
      // virtual types::Dirent *find_dentry(types::InodeID pino, std::string name, bool onlyLocal) = 0;
      // virtual types::Dirent *get_dentry(types::InodeID ino) = 0;
      // virtual types::ErrCode remove_dentry(types::InodeID pino, std::string name) = 0;
      // virtual std::string get_dentry_path(types::InodeID ino) = 0;

      // virtual types::Inode* get_inode(types::InodeID ino) = 0;
      // virtual types::ErrCode update_inode_size(types::InodeID ino, uint64_t size, bool sync) = 0;
      // virtual types::ErrCode refresh_inode(types::InodeID ino) = 0;
      // virtual types::ErrCode remove_inode(types::InodeID ino) = 0;

      // virtual types::ErrCode rename(types::InodeID oldPino, std::string oldName, types::InodeID newPino, std::string newName) = 0;
      // virtual types::ErrCode load_sub_dentires(types::InodeID ino, types::Dirent dst[]) = 0;
      // virtual void destory() = 0;
    };

    class LocalMeta {};
  }
}
#endif
