#ifndef CATFS_FS_OPENFILE_H_
#define CATFS_FS_OPENFILE_H_

#include <condition_variable>
#include <atomic>
#include <memory>
#include <string>
#include "fs/freader/freader.h"
#include "fs/fwriter/fwriter.h"
#include "stor/stor.h"
#include "types/handle.h"
#include "types/inode.h"
#include "types/rtfile.h"

namespace catfs {
namespace fs {
class OpenFile {
 private:
  bool is_writing = false;
  std::atomic<int32_t> first_write = 0;
  std::condition_variable writeDoneCond;  // 用来通知读端写入已经结束
  std::shared_ptr<stor::Stor> stor;

 public:
  std::string name;
  types::InodeID ino;
  types::InodeID pino;
  types::HandleID hno;

  FReader *reader;
  FWriter *writer;
  std::shared_ptr<types::RTFile> rtfile;
  bool has_wrote;

  OpenFile(types::InodeID ino, types::HandleID hno, std::shared_ptr<types::RTFile> file) {
    this->ino = ino;
    this->hno = hno;
    this->rtfile = file;
  }

  ~OpenFile() { 
    free(reader);
    free(writer);
  }

  int read(uint64_t off, uint64_t size, char *buf);
  int write(uint64_t off, uint64_t size, const char *buf);
  void release();
};
}  // namespace fs
}  // namespace catfs

#endif
