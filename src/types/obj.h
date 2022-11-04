#ifndef CATFS_OBJ_H_
#define CATFS_OBJ_H_

#include <string>

namespace catfs
{
  namespace types
  {
    struct ObjInfo
    {
      std::string name;
      uint64_t size;
      timespec ctime;
      timespec mtime;
      uint32_t *uid;
      uint32_t *gid;
      mode_t *mode;
      bool is_dir;
    };
  }
}
#endif
