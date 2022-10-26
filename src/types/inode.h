#ifndef CATFS_INODE_H_
#define CATFS_INODE_H_

#include <stdint.h>
#include <ctime>

namespace catfs
{
  namespace types
  {
    using InodeID = uint64_t;
    using FileMode = uint32_t;

    static const InodeID ROOT_INODE_ID = 1;

    class Inode
    {
    public:
      InodeID ino;
      uint64_t size;
      time_t ctime;
      time_t mtime;
      FileMode mode;
      uint32_t uid;
      uint32_t gid;
    };
  }
}
#endif