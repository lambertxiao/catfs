#include "fs/open_file.h"

namespace catfs {
namespace fs {
int OpenFile::read(off_t off, size_t size, char *buf) {
  // todo 提前判断文件大小，off等条件
  return reader->read(off, size, buf);
}
}  // namespace fs
}  // namespace catfs
