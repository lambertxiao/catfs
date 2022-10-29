#ifndef CATFS_FS_OPENDIR_H_
#define CATFS_FS_OPENDIR_H_

#include <vector>
#include "types/inode.h"
#include "types/dirent.h"

namespace catfs
{
  namespace fs
  {
    using HandleID = uint64_t;

    class OpenDir
    {
    public:
      types::InodeID ino;
      HandleID hno;
      std::vector<types::Dirent> dentries;

      OpenDir(types::InodeID ino, HandleID hno)
      {
        this->ino = ino;
        this->hno = hno;
      }
    };
  }
}

#endif
