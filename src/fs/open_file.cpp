#include <atomic>
#include "fs/open_file.h"
#include "fmtlog/fmtlog.h"

namespace catfs {
namespace fs {

int OpenFile::read(off_t off, size_t size, char *buf) {
  if (off >= rtfile->size) {
    loge("fpath:{} read offset exceed file size, off:{}, fsize:{}", rtfile->path, off, rtfile->size);
    return 0;
  }

  if (off + size > rtfile->size) {
    size = rtfile->size - off;
  }

  if (size == 0) {
    return 0;
  }

  return reader->read(off, size, buf);
}

int OpenFile::write(off_t off, size_t size, const char *buf) {
  if (first_write.load() == 0) {
    int expect = 0;
    if (first_write.compare_exchange_strong(expect, 1)) {
      is_writing = true;
    }
  }
  int n = writer->write(off, size, buf);
  has_wrote = true;
  rtfile->size = writer->file_size();
  return n;
}

void OpenFile::release() {
  if (writer != NULL) {
    writer->release();
    is_writing = false;
  }

  if (reader != NULL) {
    reader->release();
  }
}

}  // namespace fs
}  // namespace catfs
