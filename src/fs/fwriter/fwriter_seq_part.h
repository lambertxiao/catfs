#ifndef CATFS_FS_FWRITER_SEQ_PART_H_
#define CATFS_FS_FWRITER_SEQ_PART_H_

#include <memory>
#include <string>

namespace catfs {
namespace fs {

class Part {
 public:
  char* buf = NULL;
  uint64_t part_size;
  uint64_t woff;

  Part(uint64_t size) { 
    buf = (char*)malloc(size * sizeof(char)); 
    part_size = size;
    woff = 0;
  }

  ~Part() { free(buf); }

  uint64_t len() { return woff; }

  uint64_t cap() { return part_size - woff; }

  bool full() { return part_size == woff; }

  int write(const char* data, uint64_t size) {
    if (woff == part_size) {
      return 0;
    }
    memcpy(buf + woff, data, size);
    woff += size;
    return size;
  }

  uint64_t copy(Part* src, uint64_t off, uint64_t size) { return write(src->buf + off, size); }

  void clear() { woff = 0; }
};
}  // namespace fs
}  // namespace catfs
#endif
