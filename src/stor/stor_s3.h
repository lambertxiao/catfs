#ifndef CATFS_STOR_S3_H_
#define CATFS_STOR_S3_H_

// #include "minio-cpp/request.h"
// #include "minio-cpp/client.h"

#include "stor/stor.h"
#include "stor/minio.h"

namespace catfs
{
  namespace stor
  {
    class S3Stor : public Stor
    {
    private:
      StorOpt opt;
      minio::s3::BaseUrl base_url;
      minio::s3::Client* s3_client;
      minio::creds::StaticProvider *provider;

    public:
      S3Stor(const StorOpt &opt)
      {
        this->opt = opt;
        base_url = minio::s3::BaseUrl(opt.endpoint, false);
        provider = new minio::creds::StaticProvider(opt.public_key, opt.private_key);
        s3_client = new minio::s3::Client(base_url, provider);
      }
      ~S3Stor()
      {
        delete s3_client;
        delete provider;
      }

      virtual void head_file(HeadFileReq &req, HeadFileResp &resp) override;
      virtual void list_objects(ListObjectsReq &req, ListObjectsResp &resp) override;
      virtual PutFileResp *put_file(PutFileReq &req) override;
      virtual DeleteFileResp *delete_file(DeleteFileReq &req) override;
    };
  }
}

#endif
