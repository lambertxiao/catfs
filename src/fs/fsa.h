#include "fs/fuse.h"

namespace catfs {
  namespace fs {
    class FuseAdapter {
      public:
        static void statfs(fuse_req_t req, fuse_ino_t ino) {}
    };
  };
};
