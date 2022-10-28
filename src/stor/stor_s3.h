#ifndef CATFS_STOR_S3_H_
#define CATFS_STOR_S3_H_

#include "stor/stor.h"

namespace catfs
{
  namespace stor
  {
    class S3Stor : public Stor
    {
    private:
      StorOpt opt;

    public:
      S3Stor(const StorOpt &opt)
      {
        this->opt = opt;
      }

      virtual HeadFileResp *head_file(HeadFileReq *req) override;
      virtual ListObjectsResp *list_objects(ListObjectsReq *req) override;
      virtual PutFileResp *put_file(PutFileReq *req) override;
      virtual DeleteFileResp *delete_file(DeleteFileReq *req) override;
    };
  }
}

#endif
