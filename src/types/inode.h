#ifndef CATFS_TYPES_INODE_H_
#define CATFS_TYPES_INODE_H_

#include <stdint.h>
#include <ctime>
#include <fcntl.h>

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
      timespec ctime;
      timespec mtime;
      mode_t mode;
      uint32_t uid;
      uint32_t gid;
    };
  }
}
#endif