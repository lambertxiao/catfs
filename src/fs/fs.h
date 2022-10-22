#include <string>
#include "fuse.h"

namespace catfs {
  namespace fs {
    struct CatFsOpt {};

    class CFS {
      public:
        static CFS* getInstance();
        
        int statfs(const std::string path, struct statvfs* stbuf);
    };
  }
}
