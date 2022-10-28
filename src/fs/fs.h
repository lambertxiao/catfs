#ifndef CATFS_FS_FS_H_
#define CATFS_FS_FS_H_

#include <string>
#include <memory>
#include "fs/fuse.h"
#include "meta/meta.h"
#include "types/inode.h"

namespace catfs {
  namespace fs {
    using catfs::types::InodeID;
    using catfs::types::Inode;
    using catfs::types::Dentry;
    using catfs::meta::Meta;

    struct CatFsOpt {};

    class CatFS {
    private:
      std::shared_ptr<Meta> meta;
    
    public:
      CatFS(std::shared_ptr<Meta> meta) {
        this->meta = meta;
      }

      ~CatFS() {}
      
      const Inode* lookup_inode(InodeID ino);
      const Dentry* find_dentry(InodeID pino, std::string name);
      const Dentry* create_dentry(InodeID parent, std::string name, mode_t mode);
      void remove_dentry(InodeID parent, std::string name);
    };
  }
}
#endif
