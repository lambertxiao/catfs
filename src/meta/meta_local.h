#ifndef CATFS_META_LOCAL_H_
#define CATFS_META_LOCAL_H_

#include <string>
#include "meta/meta.h"
#include "stor/stor.h"

namespace catfs
{
  namespace meta
  {
    USING_TYPES

    struct InodeUpdateAttr
    {
      uint64_t *size;
      mode_t *mode;
      uint32_t *uid;
      uint32_t *gid;
    };

    // local meta is a interface,
    // can by implement by localMem or external kv storage
    class LocalMeta
    {
    public:
      virtual Inode *get_inode(InodeID ino) = 0;
      virtual Inode *update_inode(InodeID ino, InodeUpdateAttr updater, bool sync) = 0;
      virtual void remove_inode(InodeID ino) = 0;

      virtual Dentry *get_dentry(InodeID ino) = 0;
      virtual Dentry *create_dentry(InodeID pino, std::string name, Inode *inode) = 0;
      virtual Dentry *find_dentry(InodeID pino, std::string name) = 0;
      virtual Dentry *create_dentry_from_obj(InodeID pino, std::string name, stor::ObjInfo obj, bool isDir) = 0;
      virtual void remove_dentry(InodeID pino, std::string name) = 0;

      virtual void rename(InodeID src_pino, std::string src_name, InodeID dst_pino, std::string dst_name) = 0;
      virtual Inode *create_new_inode(mode_t mode, uint32_t gid, uint32_t uid) = 0;
    };
  }
}
#endif
