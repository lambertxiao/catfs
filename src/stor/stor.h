#ifndef CATFS_STOR_H_
#define CATFS_STOR_H_

#include <iostream>

using std::string;

namespace catfs {
  namespace stor {

    struct ObjInfo {};

    struct HeadFileReq {
      string objKey;
    };
    struct HeadFileResp {
      ObjInfo obj;
    };

    struct StorOpt {
      string bucket;
      string public_key;
      string private_key;
      string endpoint;
    };

    class Stor {
    public:
      virtual HeadFileResp* HeadFile(HeadFileReq* req) = 0;
    };
  }
}

#endif
