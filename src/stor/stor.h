#ifndef CATFS_STOR_H_
#define CATFS_STOR_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <map>

using std::string;

namespace catfs
{
  namespace stor
  {

    struct ObjInfo
    {
      uint64_t size;
      timespec ctime;
      timespec mtime;
      uint32_t *uid;
      uint32_t *gid;
      mode_t *mode;
      bool is_dir;
    };

    struct HeadFileReq
    {
      string obj_key;
    };
    struct HeadFileResp
    {
      ObjInfo *obj;
    };

    struct ListObjectsReq
    {
      string delimiter;
      string prefix;
      uint32_t max;       
      string marker;
    };

    struct ListObjectsResp
    {
      bool is_trunc;
	    string marker;         
      std::vector<ObjInfo> objs;       
    };

    struct PutFileReq
    {
      string obj_key;
      std::ifstream buf;
      std::map<string, string> meta_data;
    };

    struct PutFileResp
    {
      string etag;
    };

    struct DeleteFileReq
    {
      string obj_key;
    };

    struct DeleteFileResp
    {

    };

    struct StorOpt
    {
      string bucket;
      string public_key;
      string private_key;
      string endpoint;
    };

    class Stor
    {
    public:
      virtual HeadFileResp *head_file(HeadFileReq &req) = 0;
      virtual ListObjectsResp *list_objects(ListObjectsReq &req) = 0;
      virtual PutFileResp *put_file(PutFileReq &req) = 0;
      virtual DeleteFileResp *delete_file(DeleteFileReq &req) = 0;
    };
  }
}

#endif
