#ifndef CATFS_TYPES_DIRENT_H_
#define CATFS_TYPES_DIRENT_H_

#include <string>
#include "inode.h"

namespace catfs {
namespace types {

class Dirent {
 public:
  std::string name;
  // InodeID pino;
  Inode *inode;
};

}  // namespace types
}  // namespace catfs
#endif