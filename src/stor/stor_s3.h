#ifndef CATFS_STOR_S3
#define CATFS_STOR_S3

#include "stor/stor.h"

namespace catfs {
  namespace stor {
    class S3Stor : public Stor {
    public:
      S3Stor() {};
      HeadFileResp* HeadFile(HeadFileReq* req) override;
    };
  }
}

#endif
