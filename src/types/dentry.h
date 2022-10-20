#ifndef CATFS_DENTRY_H
#define CATFS_DENTRY_H
#include <string>
#include <ctime>
#include <mutex>
#include <map> 

namespace catfs{
namespace types {
  class Dentry {
    public:
      std::mutex lock;
      std::string name;
      Dentry* parent;
      Inode *inode;
      time_t ttl;
      bool synced;
      std::map<std::string, Dentry*> children;
  };
} // namespace types
} // namespace catfs
#endif