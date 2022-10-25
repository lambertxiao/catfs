#ifndef CATFS_STOR_H
#define CATFS_STOR_H

#include <iostream>

namespace catfs {
  namespace stor {

    struct ObjInfo {};

    struct HeadFileReq {
      std::string objKey;
    };
    struct HeadFileResp {
      ObjInfo obj;
    };

    class Stor {
    public:
      virtual HeadFileResp* HeadFile(HeadFileReq* req) = 0;
    };
  }
}

#endif
