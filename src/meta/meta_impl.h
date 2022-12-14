#ifndef CATFS_META_IMPL_H_
#define CATFS_META_IMPL_H_

#include <ctime>
#include <memory>
#include <unordered_map>
#include <vector>

#include "meta/meta.h"
#include "meta/meta_local.h"
#include "stor/stor.h"
#include "types/dirent.h"
#include "types/ftreenode.h"
#include "types/obj.h"

namespace catfs {
namespace meta {
USING_TYPES

class MetaImpl : public Meta {
 private:
  MetaOpt opt;
  std::shared_ptr<LocalMeta> local_meta;
  std::shared_ptr<stor::Stor> stor;

 public:
  MetaImpl(const MetaOpt &opt, std::shared_ptr<LocalMeta> lm, std::shared_ptr<stor::Stor> stor) {
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
  void update_inode_size(InodeID ino, uint64_t size, bool sync) override;

  void get_remote_obj(Dentry &parent, const std::string &name, types::ObjInfo &obj, bool &exist, bool &is_dir);
  bool is_remote_dir_exist(const std::string &path);
  void refresh_sub_dentries(Dentry &dentry, bool recursive);
  void build_ftree_from_listobjects(std::string &rep_prefix, stor::ListObjectsResp &ret, types::FTreeNode &root);
};
}  // namespace meta
}  // namespace catfs
#endif
