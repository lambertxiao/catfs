#ifndef CATFS_FS_FWRITER_SEQ_H_
#define CATFS_FS_FWRITER_SEQ_H_

#include <memory>
#include <mutex>
#include <string.h>
#include "fs/fwriter/fwriter.h"
#include "types/rtfile.h"
#include "types/handle.h"

namespace catfs {
namespace fs {

class Part {
  public:
  char* buf;
  size_t size;
  int woff;

  Part(size_t size) {
    buf = (char*)malloc(size*sizeof(char));
  }
  ~Part() {
    free(buf);
  }

  uint64_t len() {
    return woff;
  }

  uint64_t cap() {
    return size - woff;
  }

  bool full() {
    return size == woff;
  }

  int write(const char* data, size_t size) {
    memcpy(buf + woff, data, size);
    woff += size;
    return size;
  }

  uint64_t copy(Part *src, off_t off, size_t size) {
    return write(src->buf + off, size);
  }

  void clear() {
    woff = 0;
  }
};

// 上传类型，小文件或大文件
enum UpType {
  SMALL, BIG
};

const static uint64_t MB_4 = 4 * 1024 * 1024;

class SequenceWriter : public FWriter {
 private:
  std::shared_ptr<types::RTFile> file;
  std::mutex flock;
  types::HandleID hno;
  Part *part_small = NULL;
  Part *part_big = NULL;
  UpType uptype = SMALL;
  bool is_writing;
  off_t next_write_offset;

 public:
  SequenceWriter(std::shared_ptr<types::RTFile> file) { this->file = file; };
  int write(off_t off, size_t size, const char *dst) override;
  void switch_bigup_mode();
  void big_upload(Part *part, bool islast);
};

}  // namespace fs
}  // namespace catfs

#endif
