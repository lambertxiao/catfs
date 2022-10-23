#include <string>
#include "fuse.h"

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
        int statfs(const std::string path, struct statvfs* stbuf);
        
        static CatFS& getInstance() {
          static CatFS instance;
          return instance;
        }
    };
  }
}
