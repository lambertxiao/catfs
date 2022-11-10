#include <iostream>
#include "fs/freader/freader_direct.h"

namespace catfs
{
  namespace fs
  {
    int DirectReader::read(off_t off, size_t size, char *dst)
    {
      auto req = stor::ReadFileReq{
        obj_key: file->path,
        off: off,
        size: size,
      };

      auto resp = stor::ReadFileResp{dst: dst};
      stor->read_file(req, resp);
      return resp.bytes;
    };
  }
}