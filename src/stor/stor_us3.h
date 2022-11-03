#ifndef CATFS_STOR_US3_H_
#define CATFS_STOR_US3_H_

#include "stor/stor.h"

namespace catfs
{
  namespace stor
  {
    class US3Stor : public Stor
    {
    private:
      StorOpt opt;

    public:
      US3Stor(const StorOpt &opt)
      {
        this->opt = opt;
      }

      virtual HeadFileResp *head_file(HeadFileReq &req) override;
      virtual ListObjectsResp *list_objects(ListObjectsReq &req) override;
    };
  }
}

#endif
