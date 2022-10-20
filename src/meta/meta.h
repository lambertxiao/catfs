#include "types/inode.h"
#include "types/dentry.h"
#include "types/dirent.h"
#include "types/errors.h"

#include <string>

namespace catfs
{
class Meta {
public:
  virtual types::Dirent* createDentry(types::InodeID pino, std::string name, types::FileMode mode) = 0;
  virtual types::Dirent* findDentry(types::InodeID pino, std::string name, bool onlyLocal) = 0;
  virtual types::Dirent* getDentry(types::InodeID ino) = 0;
  virtual types::ErrCode removeDentry(types::InodeID pino, std::string name) = 0;
  virtual std::string getDentryPath(types::InodeID ino) = 0;

  virtual types::Inode* getInode(types::InodeID ino) = 0;
  virtual types::ErrCode updateInodeSize(types::InodeID ino, uint64_t size, bool sync) = 0;
  virtual types::ErrCode refreshInode(types::InodeID ino) = 0;
  virtual types::ErrCode removeInode(types::InodeID ino) = 0;

  virtual types::ErrCode rename(types::InodeID oldPino, std::string oldName, types::InodeID newPino, std::string newName) = 0;
  virtual types::ErrCode LoadSubDentires(types::InodeID ino, types::Dirent dst[]) = 0;
  virtual void destory() = 0;
};
} // namespace catfs
