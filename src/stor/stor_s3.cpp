#include "fmtlog/fmtlog.h"

#include "aws/core/Aws.h"
#include "aws/s3/S3Client.h"
#include "aws/s3/model/HeadObjectRequest.h"
#include "aws/s3/model/ListObjectsRequest.h"
#include "aws/s3/model/PutObjectRequest.h"
#include "aws/s3/model/DeleteObjectRequest.h"
#include "aws/core/http/HttpResponse.h"

#include "stor/stor.h"
#include "stor/stor_s3.h"
#include "types/errors.h"
#include "types/obj.h"

using namespace Aws::S3;

namespace catfs
{
  namespace stor
  {
    void S3Stor::head_file(HeadFileReq &req, HeadFileResp &resp)
    {
      logi("s3stor headfile path:{}", req.obj_key);

      auto head_req = Model::HeadObjectRequest{};
      head_req.SetBucket(opt.bucket);
      head_req.SetKey(req.obj_key);

      auto head_resp = s3_client->HeadObject(head_req);

      if (!head_resp.IsSuccess())
      {
        auto resp_code = head_resp.GetError().GetResponseCode();
        if (resp_code == Aws::Http::HttpResponseCode::NOT_FOUND)
        {
          resp.exist = false;
          return;
        };

        auto err_msg = head_resp.GetError().GetMessage();
        loge("s3stor headfile error, path:{} err:{}", req.obj_key, err_msg);
        throw types::ERR_SERVER_ERROR(err_msg);
      }

      auto ret_obj = head_resp.GetResult();
      types::ObjInfo obj;
      obj.size = ret_obj.GetContentLength();
      obj.ctime.tv_sec = ret_obj.GetLastModified().Seconds();
      obj.mtime.tv_sec = ret_obj.GetLastModified().Seconds();
      resp.exist = true;
      resp.obj = obj;
    }

    void S3Stor::list_objects(ListObjectsReq &req, ListObjectsResp &resp)
    {
      auto list_req = Model::ListObjectsRequest{};
      list_req.SetBucket(opt.bucket);
      list_req.SetDelimiter("/");
      list_req.SetMarker(req.marker);
      list_req.SetMaxKeys(req.max);
      list_req.SetPrefix(req.prefix);

      auto ret = s3_client->ListObjects(list_req);

      if (!ret.IsSuccess())
      {
        auto err_msg = ret.GetError().GetMessage();
        loge("s3stor listobjects error, path:{} err:{}", req.prefix, err_msg);
        throw types::ERR_SERVER_ERROR(err_msg);
      }

      auto result = ret.GetResult();
      resp.is_trunc = result.GetIsTruncated();
      resp.next_marker = result.GetNextMarker();

      for (auto &prefix : result.GetCommonPrefixes())
      {
        resp.common_prefixes.push_back(prefix.GetPrefix());
      }

      for (auto &item : result.GetContents())
      {
        types::ObjInfo obj;
        obj.size = item.GetSize();
        obj.mtime.tv_sec = item.GetLastModified().Seconds();
        obj.ctime.tv_sec = item.GetLastModified().Seconds();
        obj.name = item.GetKey();

        resp.objs.push_back(obj);
      }
    }

    PutFileResp *S3Stor::put_file(PutFileReq &req)
    {
      auto put_req = Model::PutObjectRequest{};
      put_req.SetBucket(opt.bucket);
      put_req.SetKey(req.obj_key);

      // auto inputData = Aws::MakeShared<Aws::FStream>(
      //   "SampleAllocationTag", "", std::ios_base::in | std::ios_base::binary);
      // auto inputData = Aws::MakeShared<Aws::IOStream>(req.buf);
      // put_req.SetBody(inputData);

      return NULL;
    }
    DeleteFileResp *S3Stor::delete_file(DeleteFileReq &req)
    {
      auto delete_req = Model::DeleteObjectRequest();
      delete_req.SetBucket(opt.bucket);
      delete_req.SetKey(req.obj_key);
      auto delete_resp = s3_client->DeleteObject(delete_req);

      return NULL;
    }
  }
}
