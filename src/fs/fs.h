#include <string>
#include "fs/fuse.h"
#include "types/inode.h"

namespace catfs {
  namespace fs {
    struct CatFsOpt {};

    class CatFS {
      private:
        CatFS () = default;
        ~CatFS () = default;
        CatFS (const CatFS &)= delete;
        CatFS & operator=(const CatFS &)= delete;

      public:
        catfs::types::InodeID lookupInode(catfs::types::InodeID ino);

        static CatFS& getInstance() {
          static CatFS instance;
          return instance;
        }
    };
  }
}
