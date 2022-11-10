#ifndef CATFS_FS_OPENDIR_H_
#define CATFS_FS_OPENDIR_H_

#include "types/dirent.h"
#include "types/handle.h"
#include "types/inode.h"
#include <vector>

namespace catfs {
namespace fs {
class OpenDir {
public:
  types::InodeID ino;
  types::HandleID hno;
  std::vector<types::Dirent> dentries;

  OpenDir(types::InodeID ino, types::HandleID hno) {
    this->ino = ino;
    this->hno = hno;
  }
};
} // namespace fs
} // namespace catfs

#endif
