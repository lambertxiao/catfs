#ifndef CATFS_STOR_S3_H
#define CATFS_STOR_S3_H

#include "stor/stor.h"

namespace catfs {
  namespace stor {
    class S3Stor : public Stor {
    public:
      HeadFileResp* HeadFile(HeadFileReq* req) override;
    };
  }
}

#endif
