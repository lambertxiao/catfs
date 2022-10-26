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
      
      const Inode* lookupInode(InodeID ino);
    };
  }
}
#endif
