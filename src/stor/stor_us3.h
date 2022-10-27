#ifndef CATFS_STOR_US3_H_
#define CATFS_STOR_US3_H_

#include "stor/stor.h"

namespace catfs {
  namespace stor {
    class US3Stor : public Stor {
    public:
      US3Stor(const StorOpt& opt) {
        this->opt = opt;
      }

      virtual HeadFileResp* HeadFile(HeadFileReq* req) override;

    private:
      StorOpt opt;
    };
  }
}

#endif
