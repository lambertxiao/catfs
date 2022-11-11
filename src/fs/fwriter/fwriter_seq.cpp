#include <memory>
#include "fmtlog/fmtlog.h"
#include "fs/fwriter/fwriter_seq.h"
#include "stor/stor.h"

namespace catfs {
namespace fs {

int SequenceWriter::write(off_t off, size_t size, const char *data) {
  std::lock_guard lock(flock);

  if (next_write_offset != off) {
    auto errmsg = fmt::format(
      "fpath:{} only support sequential write, next_write_offset:{}, offset:{}",
      file->path, next_write_offset, off);
    loge(errmsg);
    throw errmsg;
  }

  uint64_t wbytes = 0;

  while (wbytes != size) {
    switch (uptype)
    {
    case SMALL:
      if (part_small == NULL) {
        part_small  = new Part(MB_4);
      }

      wbytes += part_small->write(data, size);
      if (part_small->full()) {
        // 不够写
        if (size > part_small->len()) {
          switch_bigup_mode();
        } else {
          return wbytes;
        }
      } else {

      }
      break;
    case BIG:
      wbytes += part_big->write(data, size);

      if (part_big->full()) {
        big_upload(part_big, false);
        part_big->clear();
      }

      break;
    default:
      break;
    }
  }
};

void SequenceWriter::switch_bigup_mode() {
  auto init_req = stor::MInitReq{
    obj_key: file->path,
  };
  auto init_resp = stor::MInitResp{};
  stor->minit(init_req, init_resp);

  if (init_resp.part_size != MB_4) {
    logi("fpath:{} minit with partsize:{}", file->path, init_resp.part_size);
  }

  part_big = new Part(init_resp.part_size);
  uptype = BIG;

  if (part_big->cap() == part_small->len()) {
    big_upload(part_small, false);
  } else {
    uint64_t copy_bytes = 0;
    uint64_t copy_total_bytes = part_small->len();

    while (copy_bytes < part_small->len()) {
      copy_bytes += part_big->copy(part_small, copy_bytes, copy_total_bytes - copy_bytes);

      if (part_big->full()) {
        big_upload(part_big, false);
        part_big->clear();
      }
    }
  }
  free(part_small);
}

}  // namespace fs
}  // namespace catfs
