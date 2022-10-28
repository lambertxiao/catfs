#ifndef CATFS_META_LOCAL_MEM_H_
#define CATFS_META_LOCAL_MEM_H_

#include <mutex>
#include <shared_mutex>
#include <shared_mutex>
#include <map>

#include "meta/meta_local.h"
#include "stor/stor.h"

namespace catfs
{
  namespace meta
  {
    USING_TYPES

    const uint32_t LOCK_COUNT = 64;

    class LocalMemMeta : public LocalMeta
    {
    private:
      MetaOpt opt;
      InodeID next_inode_id;
      Dentry *root_dentry;
      std::shared_mutex next_inode_id_locker;
      mutable std::shared_mutex m_locks[LOCK_COUNT];        // 多段锁
      std::map<InodeID, Dentry *> dentry_index[LOCK_COUNT]; // dentry的索引
    public:
      LocalMemMeta(MetaOpt &opt);

      void save_dentry_index(Dentry *);
      Dentry *LocalMemMeta::add_child_for_dentry(Dentry *parent, std::string name, Inode *inode);
      Inode *obj2inode(stor::ObjInfo& obj, bool is_dir);
      InodeID get_next_inode_id();

      Inode *get_inode(InodeID ino) override;
      Inode *update_inode(InodeID ino, InodeUpdateAttr updater, bool sync) override;
      void *remove_inode(InodeID ino) override;
      Dentry *get_dentry(InodeID ino) override;
      Dentry *create_dentry(InodeID pino, std::string name, Inode *inode) override;
      Dentry *find_dentry(InodeID pino, std::string name) override;
      Dentry *create_dentry_from_obj(InodeID pino, std::string name, stor::ObjInfo obj, bool isDir) override;
      void remove_dentry(InodeID pino, std::string name) override;
      void rename(InodeID src_pino, std::string src_name, InodeID dst_pino, std::string dst_name) override;
      Inode *create_new_inode(mode_t mode, uint32_t gid, uint32_t uid) override;
    };
  }
}
#endif
