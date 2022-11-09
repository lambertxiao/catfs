#ifndef CATFS_TYPES_RTFILE_H_
#define CATFS_TYPES_RTFILE_H_

#include <string>

namespace catfs
{
  namespace types
  {
    // runtime file，记录正常读写的文件
    class RTFile
    {
    public:
      std::string path;
      uint64_t size;
      RTFile(std::string path, uint64_t size)
      {
        this->path = path;
        this->size = size;
      }
    };
  }
}
#endif
