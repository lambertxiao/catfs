#include <string>
#include <ctime>
#include <mutex>
#include <map> 

namespace types {
  class Dentry {
    public:
      mutex lock;
      string name;
      Dentry* parent;
      Inode *inode;
      time_t ttl;
      bool synced;
      map<string, Dentry*> children;
  };
};
