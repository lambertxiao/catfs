#ifndef CATFS_FS_FWRITER_H_
#define CATFS_FS_FWRITER_H_

#include "stor/stor.h"

namespace catfs {
namespace fs {

class FWriter {
 protected:
  std::shared_ptr<stor::Stor> stor;
 public:
  virtual int write(off_t off, size_t size, const char *data) = 0;
  void set_stor(std::shared_ptr<stor::Stor> stor) { this->stor = stor; }
};

}  // namespace fs
}  // namespace catfs

#endif
