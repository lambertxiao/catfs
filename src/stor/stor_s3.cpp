#include "fmtlog/fmtlog.h"

#include "minio-cpp/args.h"

#include "stor/stor.h"
#include "stor/stor_s3.h"

namespace catfs
{
  namespace stor
  {
    HeadFileResp *S3Stor::head_file(HeadFileReq &req)
    {
      logi("s3stor headfile path:{}", req.obj_key);

      minio::s3::StatObjectArgs args;
      args.bucket = opt.bucket;
      args.object = req.obj_key;

      auto resp = client->StatObject(args);

      if (resp)
      {

      }

      logi("s3stor headfile path:{} resp:{}", req.obj_key, resp);
      return NULL;
    }

    ListObjectsResp *S3Stor::list_objects(ListObjectsReq &req)
    {
      return NULL;
    }

    PutFileResp *S3Stor::put_file(PutFileReq &req)
    {
      return NULL;
    }
    DeleteFileResp *S3Stor::delete_file(DeleteFileReq &req)
    {
      return NULL;
    }
  }
}
