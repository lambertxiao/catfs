#ifndef CATFS_FS_FREADER_DIRECT_H_
#define CATFS_FS_FREADER_DIRECT_H_

#include <memory>
#include "fs/freader/freader.h"
#include "stor/stor.h"
#include "types/rtfile.h"

namespace catfs {
namespace fs {
// 直读服务端的数据，不做任何缓存
class DirectReader : public FReader {
 private:
  std::shared_ptr<types::RTFile> file;

 public:
  DirectReader(std::shared_ptr<types::RTFile> file) { this->file = file; };

  int read(uint64_t off, uint64_t size, char *dst) override;
  void release() override;
};
}  // namespace fs
}  // namespace catfs

#endif
