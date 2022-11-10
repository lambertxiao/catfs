#ifndef CATFS_FS_FREADER_DIRECT_H_
#define CATFS_FS_FREADER_DIRECT_H_

#include <memory>
#include "stor/stor.h"
#include "types/rtfile.h"
#include "fs/freader/freader.h"

namespace catfs
{
  namespace fs
  {
    // 直读服务端的数据，不做任何缓存
    class DirectReader : public FReader
    {
    private:
      std::shared_ptr<types::RTFile> file;

    public:
      DirectReader(std::shared_ptr<types::RTFile> file) {
        this->file = file;
      };

      int read(off_t off, size_t size, char *dst) override;
    };
  }
}

#endif
