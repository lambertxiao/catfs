#ifndef CATFS_FS_FREADER_H_
#define CATFS_FS_FREADER_H_

#include "stor/stor.h"

namespace catfs {
namespace fs {
class FReader {
 protected:
  std::shared_ptr<stor::Stor> stor;

 public:
  virtual int read(off_t off, size_t size, char *dst) = 0;
  virtual void release() = 0;

  void set_stor(std::shared_ptr<stor::Stor> stor) { this->stor = stor; }
};
}  // namespace fs
}  // namespace catfs

#endif
