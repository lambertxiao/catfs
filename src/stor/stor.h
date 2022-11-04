#ifndef CATFS_STOR_H_
#define CATFS_STOR_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <map>

#include <types/obj.h>

using std::string;

namespace catfs
{
  namespace stor
  {

    struct HeadFileReq
    {
      string obj_key;
    };
    struct HeadFileResp
    {
      bool exist;
      types::ObjInfo obj;
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
      std::vector<types::ObjInfo> objs;
      std::vector<string> common_prefixes;
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
      virtual void head_file(HeadFileReq &req, HeadFileResp &resp) = 0;
      virtual void list_objects(ListObjectsReq &req, ListObjectsResp &resp) = 0;
      virtual PutFileResp *put_file(PutFileReq &req) = 0;
      virtual DeleteFileResp *delete_file(DeleteFileReq &req) = 0;
    };
  }
}

#endif
