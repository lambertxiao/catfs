#include "fsa.h"
#include "fs.h"

namespace catfs {
  namespace fs {
    int FuseAdapter::statfs(const char* path, struct statvfs* stbuf) {
      return CFS::getInstance()->statfs(path, stbuf);
    }
  }
}
