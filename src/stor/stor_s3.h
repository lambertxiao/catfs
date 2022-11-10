#ifndef CATFS_STOR_S3_H_
#define CATFS_STOR_S3_H_

#include "aws/core/Aws.h"
#include "aws/core/auth/AWSCredentialsProvider.h"
#include "aws/s3/S3Client.h"

#include "stor/stor.h"

namespace catfs
{
  namespace stor
  {
    class S3Stor : public Stor
    {
    private:
      StorOpt opt;
      Aws::S3::S3Client* s3_client;

    public:
      S3Stor(const StorOpt &opt)
      {
        this->opt = opt;

        // 初始化API
        Aws::SDKOptions options;
        Aws::InitAPI(options);

        Aws::Client::ClientConfiguration cfg;
        cfg.endpointOverride = opt.endpoint;
        cfg.scheme = Aws::Http::Scheme::HTTP;
        cfg.verifySSL = false;

        Aws::Auth::AWSCredentials cred(opt.public_key, opt.private_key);
        this->s3_client = new Aws::S3::S3Client(cred, cfg);
      }

      ~S3Stor()
      {
        delete s3_client;
      }

      virtual void head_file(HeadFileReq &req, HeadFileResp &resp) override;
      virtual void list_objects(ListObjectsReq &req, ListObjectsResp &resp) override;
      virtual void put_file(PutFileReq &req, PutFileResp &resp) override;
      virtual void read_file(ReadFileReq &req, ReadFileResp &resp) override;
      virtual void delete_file(DeleteFileReq &req, DeleteFileResp &resp) override;
    };
  }
}

#endif
