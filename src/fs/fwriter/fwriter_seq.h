#ifndef CATFS_FS_FWRITER_SEQ_H_
#define CATFS_FS_FWRITER_SEQ_H_

#include <memory>
#include <mutex>
#include <string.h>
#include "fs/fwriter/fwriter.h"
#include "fs/fwriter/fwriter_seq_part.h"
#include "types/rtfile.h"
#include "types/handle.h"

namespace catfs {
namespace fs {

// 上传类型，小文件或大文件
enum UpType {
  PUT, MPUT
};

const static uint64_t MB_8 = 8 * 1024 * 1024;

struct MPUTCtx {
  string upload_id;
  int part_num;
};

class SequenceWriter : public FWriter {
 private:
  std::shared_ptr<types::RTFile> file;
  std::mutex flock;
  types::HandleID hno;
  Part *put_part = NULL;
  Part *mput_part = NULL;
  MPUTCtx mput_ctx;

  UpType uptype = PUT;
  bool is_writing;
  off_t next_write_offset;

 public:
  SequenceWriter(std::shared_ptr<types::RTFile> file) { this->file = file; };
  
  int write(off_t off, size_t size, const char *dst) override;
  void write_data(const char *data, size_t size);
  void release() override;

  uint64_t file_size() override;

  void switch_mput_mode();
  void upload_part(Part *part, bool islast);
};

}  // namespace fs
}  // namespace catfs

#endif
