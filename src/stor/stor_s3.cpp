#include "fmtlog/fmtlog.h"

#include "stor/minio.h"
#include "stor/stor.h"
#include "stor/stor_s3.h"
#include "types/errors.h"
#include "types/obj.h"

namespace catfs
{
  namespace stor
  {
    void S3Stor::head_file(HeadFileReq &req, HeadFileResp &resp)
    {
      logi("s3stor headfile path:{}", req.obj_key);

      minio::s3::StatObjectArgs args;
      args.bucket = opt.bucket;
      args.object = req.obj_key;

      auto statResp = s3_client->StatObject(args);
      if (!statResp)
      {
        loge("s3stor headfile error, path:{} code:{} msg:{}", req.obj_key, statResp.status_code, statResp.message);
        if (statResp.status_code == 404)
          return;

        throw types::ERR_SERVER_ERROR(statResp.message);
      }

      types::ObjInfo obj;
      obj.size = statResp.size;
      obj.ctime.tv_sec = statResp.last_modified;
      obj.mtime.tv_sec = statResp.last_modified;
      resp.exist = true;
      resp.obj = obj;
    }

    void S3Stor::list_objects(ListObjectsReq &req, ListObjectsResp &resp)
    {
      minio::s3::ListObjectsArgs args;
      args.bucket = opt.bucket;
      args.delimiter = "/";
      args.marker = req.marker;
      args.max_keys = req.max;

      auto ret = s3_client->ListObjects(args);

      //
      //
      logi("s3stor list_objects ret");

      for (; ret; ret++) {
        minio::s3::Item item = *ret;
        if (item) {
          types::ObjInfo obj;
          obj.name = item.name;
          obj.size = item.size;
          obj.is_dir = item.is_prefix;
          resp.objs.push_back(obj);
        } else {
          loge("s3stor list_objects error, prefix:{} err:{}", req.prefix, item.Error().String());
          throw types::ERR_SERVER_ERROR(item.Error().String());
        }
      }
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
