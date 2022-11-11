#ifndef CATFS_FS_FWRITER_SEQ_PART_H_
#define CATFS_FS_FWRITER_SEQ_PART_H_

#include <memory>
#include <string>

namespace catfs {
namespace fs {

class Part {
 public:
  char* buf;
  size_t size;
  off_t woff;

  Part(size_t size) { buf = (char*)malloc(size * sizeof(char)); }
  ~Part() { free(buf); }

  uint64_t len() { return woff; }

  uint64_t cap() { return size - woff; }

  bool full() { return size == woff; }

  int write(const char* data, size_t size) {
    memcpy(buf + woff, data, size);
    woff += size;
    return size;
  }

  uint64_t copy(Part* src, off_t off, size_t size) { return write(src->buf + off, size); }

  void clear() { woff = 0; }
};
}  // namespace fs
}  // namespace catfs
#endif
