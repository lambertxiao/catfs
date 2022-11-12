#include "fmtlog/fmtlog.h"

#include "aws/core/Aws.h"
#include "aws/core/http/HttpResponse.h"
#include "aws/s3/S3Client.h"
#include "aws/s3/model/DeleteObjectRequest.h"
#include "aws/s3/model/GetObjectRequest.h"
#include "aws/s3/model/GetObjectResult.h"
#include "aws/s3/model/HeadObjectRequest.h"
#include "aws/s3/model/ListObjectsRequest.h"
#include "aws/s3/model/PutObjectRequest.h"
#include "aws/s3/model/UploadPartRequest.h"
#include "aws/s3/model/CreateMultipartUploadRequest.h"
#include "aws/s3/model/CompleteMultipartUploadRequest.h"

#include "stor/stor.h"
#include "stor/stor_s3.h"
#include "types/errors.h"
#include "types/obj.h"

using namespace Aws::S3;

namespace catfs {
namespace stor {
void S3Stor::head_file(HeadFileReq &req, HeadFileResp &resp) {
  logi("s3stor headfile path:{}", req.obj_key);

  auto head_req = Model::HeadObjectRequest{};
  head_req.SetBucket(opt.bucket);
  head_req.SetKey(req.obj_key);

  auto head_resp = s3_client->HeadObject(head_req);

  if (!head_resp.IsSuccess()) {
    auto resp_code = head_resp.GetError().GetResponseCode();
    if (resp_code == Aws::Http::HttpResponseCode::NOT_FOUND) {
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

void S3Stor::list_objects(ListObjectsReq &req, ListObjectsResp &resp) {
  auto list_req = Model::ListObjectsRequest{};
  list_req.SetBucket(opt.bucket);
  list_req.SetDelimiter("/");
  list_req.SetMarker(req.marker);
  list_req.SetMaxKeys(req.max);
  list_req.SetPrefix(req.prefix);

  auto ret = s3_client->ListObjects(list_req);

  if (!ret.IsSuccess()) {
    auto err_msg = ret.GetError().GetMessage();
    loge("s3stor listobjects error, path:{} err:{}", req.prefix, err_msg);
    throw types::ERR_SERVER_ERROR(err_msg);
  }

  auto result = ret.GetResult();
  resp.is_trunc = result.GetIsTruncated();
  resp.next_marker = result.GetNextMarker();

  for (auto &prefix : result.GetCommonPrefixes()) {
    resp.common_prefixes.push_back(prefix.GetPrefix());
  }

  for (auto &item : result.GetContents()) {
    types::ObjInfo obj;
    obj.size = item.GetSize();
    obj.mtime.tv_sec = item.GetLastModified().Seconds();
    obj.ctime.tv_sec = item.GetLastModified().Seconds();
    obj.name = item.GetKey();

    resp.objs.push_back(obj);
  }
}

void S3Stor::put_file(PutFileReq &req, PutFileResp &resp) {
  auto put_req = Model::PutObjectRequest{};
  put_req.SetBucket(opt.bucket);
  put_req.SetKey(req.obj_key);

  auto inputData = Aws::MakeShared<Aws::StringStream>("");
  if (req.buf != NULL) inputData->write(req.buf, req.size);

  put_req.SetBody(inputData);

  auto outcome = s3_client->PutObject(put_req);
  if (!outcome.IsSuccess()) {
    auto err_msg = outcome.GetError().GetMessage();
    loge("s3stor putobject error:{}", err_msg);
    throw types::ERR_SERVER_ERROR(err_msg);
  }
  logi("s3stor put_file {} done", req.obj_key);
}

void S3Stor::delete_file(DeleteFileReq &req, DeleteFileResp &resp) {
  auto delete_req = Model::DeleteObjectRequest();
  delete_req.SetBucket(opt.bucket);
  delete_req.SetKey(req.obj_key);
  auto delete_resp = s3_client->DeleteObject(delete_req);

  if (!delete_resp.IsSuccess()) {
    auto err_msg = delete_resp.GetError().GetMessage();
    loge("s3stor delete_file error:{}", err_msg);
    throw types::ERR_SERVER_ERROR(err_msg);
  }

  logi("s3stor delete_file {} done", req.obj_key);
}

void S3Stor::read_file(ReadFileReq &req, ReadFileResp &resp) {
  auto get_req = Model::GetObjectRequest();
  auto range = fmt::format("bytes={}-{}", req.off, req.off + req.size - 1);
  logi("s3tor read {} with range {}", req.obj_key, range);
  get_req.SetBucket(opt.bucket);
  get_req.SetKey(req.obj_key);
  get_req.SetRange(range);

  Aws::S3::Model::GetObjectOutcome get_resp = s3_client->GetObject(get_req);
  if (!get_resp.IsSuccess()) {
    auto err_msg = get_resp.GetError().GetMessage();
    loge("s3stor read_file error:{}", err_msg);
    throw types::ERR_SERVER_ERROR(err_msg);
  }

  auto content_length = get_resp.GetResult().GetContentLength();
  get_resp.GetResult().GetBody().read(resp.dst, content_length);
  resp.bytes = content_length;
}

void S3Stor::minit(MInitReq &req, MInitResp &resp) {
  auto minit_req = Model::CreateMultipartUploadRequest();
  minit_req.SetBucket(opt.bucket);
  minit_req.SetKey(req.obj_key);

  auto minit_resp = s3_client->CreateMultipartUpload(minit_req);
  if (!minit_resp.IsSuccess()) {
    auto err_msg = minit_resp.GetError().GetMessage();
    loge("s3stor minit error:{}", err_msg);
    throw types::ERR_SERVER_ERROR(err_msg);
  }

  resp.upload_id = minit_resp.GetResult().GetUploadId();
  resp.part_size = 8 * 1024 * 1024;

  logi("s3stor minit fpath:{}, upload_id:{}", req.obj_key, resp.upload_id);
}

void S3Stor::mput(MPutReq &req, MPutResp &resp) {
  logi("s3stor mput fpath:{}, part_num:{}", req.obj_key, req.part_num+1);
  auto mput_req = Model::UploadPartRequest();
  mput_req.SetBucket(opt.bucket);
  mput_req.SetKey(req.obj_key);
  mput_req.SetUploadId(req.upload_id);
  mput_req.SetPartNumber(req.part_num+1);

  auto inputData = Aws::MakeShared<Aws::StringStream>("");
  inputData->write(req.data, req.size);
  mput_req.SetBody(inputData);

  auto mput_resp = s3_client->UploadPart(mput_req);
  if (!mput_resp.IsSuccess()) {
    auto err_msg = mput_resp.GetError().GetMessage();
    loge("s3stor mput error:{}", err_msg);
    throw types::ERR_SERVER_ERROR(err_msg);
  }
}

void S3Stor::mfinish(MFinishReq &req, MFinishResp &resp) {
  logi("s3stor mfinish fpath:{}, upload_id:{}", req.obj_key, req.upload_id);

  auto mfinish_req = Model::CompleteMultipartUploadRequest();
  mfinish_req.SetBucket(opt.bucket);
  mfinish_req.SetKey(req.obj_key);
  mfinish_req.SetUploadId(req.upload_id);

  auto mfinish_resp = s3_client->CompleteMultipartUpload(mfinish_req);
  if (!mfinish_resp.IsSuccess()) {
    auto err_msg = mfinish_resp.GetError().GetMessage();
    loge("s3stor mfinish error:{}", err_msg);
    throw types::ERR_SERVER_ERROR(err_msg);
  }
}

}  // namespace stor
}  // namespace catfs
