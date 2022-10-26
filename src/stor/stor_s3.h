#ifndef CATFS_STOR_S3_H_
#define CATFS_STOR_S3_H_

#include "stor/stor.h"

namespace catfs {
  namespace stor {
    class S3Stor : public Stor {
    public:
      S3Stor(const StorOpt& opt) {
        this->opt = opt;
      }

      virtual HeadFileResp* HeadFile(HeadFileReq* req) override;

    private:
      StorOpt opt;
    };
  }
}

#endif
