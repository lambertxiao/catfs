#ifndef CATFS_INODE_H
#define CATFS_INODE_H

#include <stdint.h>
#include <ctime>

namespace catfs
{
  namespace types
  {
    using InodeID = uint64_t;
    using FileMode = uint32_t;

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

  } // namespace types
} // namespace catfs
#endif