#include "types/inode.h"
#include "types/dentry.h"
#include "types/dirent.h"
#include "types/errors.h"
#include <string>

using namespace types;
using namespace std;

class Meta {
  public:
    virtual Dirent* createDentry(InodeID pino, string name, FileMode mode) = 0;
    virtual Dirent* findDentry(InodeID pino, string name, bool onlyLocal) = 0;
    virtual Dirent* getDentry(InodeID ino) = 0;
    virtual ErrCode removeDentry(InodeID pino, string name) = 0;
    virtual string getDentryPath(InodeID ino) = 0;

    virtual Inode* getInode(InodeID ino) = 0;
    virtual ErrCode updateInodeSize(InodeID ino, uint64_t size, bool sync) = 0;
    virtual ErrCode refreshInode(InodeID ino) = 0;
    virtual ErrCode removeInode(InodeID ino) = 0;

    virtual ErrCode rename(InodeID oldPino, string oldName, InodeID newPino, string newName) = 0;
    virtual ErrCode LoadSubDentires(InodeID ino, Dirent dst[]) = 0;
    virtual void destory() = 0;
};
