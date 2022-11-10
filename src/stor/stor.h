#ifndef CATFS_STOR_H_
#define CATFS_STOR_H_

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include <types/obj.h>

using std::string;

namespace catfs {
namespace stor {

struct HeadFileReq {
  string obj_key;
};
struct HeadFileResp {
  bool exist;
  types::ObjInfo obj;
};

struct ListObjectsReq {
  string delimiter;
  string prefix;
  uint32_t max;
  string marker;
};

struct ListObjectsResp {
  bool is_trunc;
  string next_marker;
  std::vector<types::ObjInfo> objs;
  std::vector<string> common_prefixes;
};

struct PutFileReq {
  string obj_key;
  char *buf = NULL;
  size_t size;
  std::map<string, string> meta_data;
};

struct PutFileResp {
  string etag;
};

struct DeleteFileReq {
  string obj_key;
};

struct DeleteFileResp {};

struct ReadFileReq {
  string obj_key;
  off_t off;
  size_t size;
};

struct ReadFileResp {
  char *dst;
  size_t bytes;
};

struct StorOpt {
  string bucket;
  string public_key;
  string private_key;
  string endpoint;
};

class Stor {
public:
  virtual void head_file(HeadFileReq &req, HeadFileResp &resp) = 0;
  virtual void list_objects(ListObjectsReq &req, ListObjectsResp &resp) = 0;
  virtual void put_file(PutFileReq &req, PutFileResp &resp) = 0;
  virtual void read_file(ReadFileReq &req, ReadFileResp &resp) = 0;
  virtual void delete_file(DeleteFileReq &req, DeleteFileResp &resp) = 0;
};
} // namespace stor
} // namespace catfs

#endif
