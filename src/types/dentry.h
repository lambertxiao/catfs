#ifndef CATFS_DENTRY_H_
#define CATFS_DENTRY_H_
#include <string>
#include <ctime>
#include <mutex>
#include <map>

namespace catfs
{
  namespace types
  {
    class Dentry
    {
    public:
      std::mutex lock;
      std::string name;
      Dentry *parent;
      Inode *inode;
      time_t ttl;
      bool synced;
      std::map<std::string, Dentry *> children;
    };
  }
}
#endif
