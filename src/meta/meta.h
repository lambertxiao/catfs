#ifndef CATFS_META_H_
#define CATFS_META_H_

#include "types/inode.h"
#include "types/dentry.h"
#include "types/dirent.h"
#include "types/errors.h"

#include <string>

#define USING_TYPES     \
  using types::InodeID; \
  using types::Inode;   \
  using types::Dentry;  \
  using types::Dirent;

namespace catfs
{
  namespace meta
  {
    USING_TYPES

    struct MetaOpt
    {
      uint32_t gid;
      uint32_t uid;
      uint32_t dcache_timeout;
      std::string bucket;
      std::string bucket_prefix;
    };

    class Meta
    {
    public:
      virtual Inode *get_inode(InodeID ino) = 0;
      virtual Dentry *find_dentry(InodeID pino, const std::string &name, bool onlyLocal) = 0;
      virtual Dentry *create_dentry(InodeID pino, const std::string &name, mode_t mode) = 0;
      virtual void remove_dentry(InodeID pino, const std::string &name) = 0;
      virtual Dentry *get_dentry(InodeID ino) = 0;
      virtual std::vector<Dirent> load_sub_dentries(InodeID ino) = 0;

      // virtual types::ErrCode remove_dentry(types::InodeID pino, std::string name) = 0;
      // virtual std::string get_dentry_path(types::InodeID ino) = 0;

      // virtual types::ErrCode update_inode_size(types::InodeID ino, uint64_t size, bool sync) = 0;
      // virtual types::ErrCode refresh_inode(types::InodeID ino) = 0;
      // virtual types::ErrCode remove_inode(types::InodeID ino) = 0;

      // virtual types::ErrCode rename(types::InodeID oldPino, std::string oldName, types::InodeID newPino, std::string newName) = 0;
      // virtual types::ErrCode load_sub_dentires(types::InodeID ino, types::Dirent dst[]) = 0;
      // virtual void destory() = 0;
    };

  }
}
#endif
