#include "fs/fs.h"

namespace catfs {
  namespace fs {
    const Inode* CatFS::lookup_inode(InodeID ino) {
      return meta->get_inode(ino);
    }

    const Dentry* CatFS::find_dentry(InodeID pino, std::string name) {
      return meta->find_dentry(pino, name);
    }
  }
}
