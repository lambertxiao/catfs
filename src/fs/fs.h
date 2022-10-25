#ifndef CATFS_FS_FS_H
#define CATFS_FS_FS_H

#include <string>
#include "fs/fuse.h"
#include "meta/meta.h"
#include "types/inode.h"

namespace catfs {
  namespace fs {
    using catfs::types::InodeID;
    using catfs::types::Inode;

    struct CatFsOpt {};

    class CatFS {
    public:
      CatFS (meta::Meta* meta);
      ~CatFS () = default;

      meta::Meta* meta;

    public:
      const Inode* lookupInode(InodeID ino);
    };
  }
}
#endif
