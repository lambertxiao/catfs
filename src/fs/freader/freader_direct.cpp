#include "fs/freader/freader_direct.h"
#include <iostream>

namespace catfs {
namespace fs {
int DirectReader::read(uint64_t off, uint64_t size, char *dst) {
  auto req = stor::ReadFileReq{
    obj_key : file->path,
    off : off,
    size : size,
  };

  auto resp = stor::ReadFileResp{dst : dst};
  stor->read_file(req, resp);
  return resp.bytes;
};

void DirectReader::release() {}

}  // namespace fs
}  // namespace catfs
