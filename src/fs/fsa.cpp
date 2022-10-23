#include "fs/fsa.h"
#include "fs/fs.h"

namespace catfs {
  namespace fs {
    int FuseAdapter::statfs(const char* path, struct statvfs* stbuf) {
      return CatFS::getInstance().statfs(path, stbuf);
    }
  }
}
