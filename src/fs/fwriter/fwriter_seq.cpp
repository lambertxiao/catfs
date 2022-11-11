#include "fs/fwriter/fwriter_seq.h"

#include <memory>

#include "fmtlog/fmtlog.h"
#include "stor/stor.h"

namespace catfs {
namespace fs {

int SequenceWriter::write(off_t off, size_t size, const char *data) {
  std::lock_guard lock(flock);

  if (next_write_offset != off) {
    auto errmsg = fmt::format("fpath:{} only support sequential write, next_write_offset:{}, offset:{}", file->path,
                              next_write_offset, off);
    loge(errmsg);
    throw errmsg;
  }

  write_data(data, size);
  next_write_offset += size;
  return size;
};

void SequenceWriter::write_data(const char *data, size_t size) {
  uint64_t wbytes = 0;
  uint64_t total = size;

  // 当没有写完时一直循环写入
  while (wbytes != total) {
    switch (uptype) {
      case PUT:
        if (put_part == NULL) {
          put_part = new Part(MB_8);
        }

        wbytes += put_part->write(data, size);

        if (put_part->full()) {
          // left_bytes一定大于等于0
          auto left_bytes = size - wbytes;

          if (left_bytes > 0) {
            // 数据还没写完，切换到分片上传模式
            switch_mput_mode();
            // 指针前移
            data += wbytes;
            size = size - wbytes;
          } else if (left_bytes == 0) {
            // 刚好写完，需要不要切换，看接下来的行为
            return;
          }
        } else if (wbytes == size) {
          // 数据全部写下去了，结束本次写入
          return;
        }

        break;
      case MPUT:
        wbytes += mput_part->write(data, size);

        if (mput_part->full()) {
          upload_part(mput_part, false);
          mput_part->clear();
        }

        // 指针前移
        data += wbytes;
        size = size - wbytes;
        break;
    }
  }
}

void SequenceWriter::switch_mput_mode() {
  auto init_req = stor::MInitReq{
    obj_key : file->path,
  };
  auto init_resp = stor::MInitResp{};
  stor->minit(init_req, init_resp);

  if (init_resp.part_size != MB_8) {
    logi("fpath:{} minit with partsize:{}", file->path, init_resp.part_size);
  }

  mput_part = new Part(init_resp.part_size);
  uptype = MPUT;
  mput_ctx.part_num = 0;
  mput_ctx.upload_id = init_resp.upload_id;

  if (mput_part->cap() == put_part->len()) {
    upload_part(put_part, false);
  } else {
    uint64_t copy_bytes = 0;
    uint64_t copy_total_bytes = put_part->len();

    while (copy_bytes < copy_total_bytes) {
      copy_bytes += mput_part->copy(put_part, copy_bytes, copy_total_bytes - copy_bytes);

      if (mput_part->full()) {
        upload_part(mput_part, false);
        mput_part->clear();
      }
    }
  }
  free(put_part);
}

void SequenceWriter::upload_part(Part *part, bool islast) {
  auto part_num = mput_ctx.part_num;
  mput_ctx.part_num++;

  auto mput_req = stor::MPutReq{
    upload_id: mput_ctx.upload_id,
    part_num: part_num,
    data: part->buf,
    size: part->len(),
  };

  auto mput_resp = stor::MPutResp{};
  stor->mput(mput_req, mput_resp);
}

void SequenceWriter::release() {
  std::lock_guard lock(flock);
}

}  // namespace fs
}  // namespace catfs
