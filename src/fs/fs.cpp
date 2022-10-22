#include "fs.h"

namespace catfs {
  namespace fs {
    CFS* getInstance() {
      // 返回单一实例

      // 1. 加载启动参数
      // 2. 初始化meta, storage,
    }

    int CFS::statfs(const std::string path, struct statvfs* stbuf) {
      return 0;
    }
  }
}
