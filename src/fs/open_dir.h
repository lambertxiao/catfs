#ifndef CATFS_FS_OPENDIR_H_
#define CATFS_FS_OPENDIR_H_

#include <vector>
#include "types/inode.h"
#include "types/dirent.h"
#include "types/handle.h"

namespace catfs
{
  namespace fs
  {
    class OpenDir
    {
    public:
      types::InodeID ino;
      types::HandleID hno;
      std::vector<types::Dirent> dentries;

      OpenDir(types::InodeID ino, types::HandleID hno)
      {
        this->ino = ino;
        this->hno = hno;
      }
    };
  }
}

#endif
