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
    private:
      // CatFS (meta::Meta* meta) = default;
      // ~CatFS () = default;
      // CatFS (const CatFS &)= delete;
      // CatFS & operator=(const CatFS &)= delete;

      meta::Meta* meta;

    public:
      static CatFS* instance;
      const Inode* lookupInode(InodeID ino);

      static CatFS* getInstance(CatFsOpt opt) {
        // if (instance == NULL) {
        //   meta::Meta* meta = new meta::MetaImpl();

        //   instance = new CatFS(meta);
        // }
        return instance;
      }
    };
  }
}
