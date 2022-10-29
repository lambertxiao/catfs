#ifndef CATFS_DIRENT_H_
#define CATFS_DIRENT_H_

#include "inode.h"
#include <string>

namespace catfs
{
  namespace types
  {

    class Dirent
    {
    public:
      std::string name;
      // InodeID pino;
      InodeID ino;
    };

  } // namespace types
} // namespace catfs
#endif