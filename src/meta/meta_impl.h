#ifndef CATFS_META_IMPL_H_
#define CATFS_META_IMPL_H_

#include <memory>
#include <ctime>
#include <vector>
#include <unordered_map>

#include "meta/meta.h"
#include "meta/meta_local.h"
#include "types/dirent.h"
#include "types/obj.h"
#include "types/ftreenode.h"
#include "stor/stor.h"

namespace catfs
{
  namespace meta
  {
    USING_TYPES

    class MetaImpl : public Meta
    {
    private:
      MetaOpt opt;
      std::shared_ptr<LocalMeta> local_meta;
      std::shared_ptr<stor::Stor> stor;

    public:
      MetaImpl(const MetaOpt &opt, std::shared_ptr<LocalMeta> lm, std::shared_ptr<stor::Stor> stor)
      {
        this->opt = opt;
        this->stor = stor;
        this->local_meta = lm;
      }

      Inode *get_inode(InodeID ino) override;
      Dentry *find_dentry(InodeID pino, const std::string &name, bool onlyLocal) override;
      Dentry *create_dentry(InodeID pino, const std::string &name, mode_t mode) override;
      void remove_dentry(InodeID pino, const std::string &name) override;
      Dentry *get_dentry(InodeID ino) override;
      void load_sub_dentries(InodeID ino, std::vector<types::Dirent> &dirents) override;

      void get_remote_obj(Dentry& parent, const std::string &name, types::ObjInfo &obj, bool &exist, bool &is_dir);
      bool is_remote_dir_exist(const std::string &path);
      void refresh_sub_dentries(Dentry& dentry, bool recursive);
      void build_ftree_from_listobjects(stor::ListObjectsResp& ret, types::FTreeNode &root);
    };
  }
}
#endif
