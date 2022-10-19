#include<stdint.h>
#include<ctime>

namespace types {
  typedef uint64_t InodeID;
  typedef uint32_t FileMode;

  class Inode {
    public:
      InodeID ino;
      uint64_t size;
      time_t ctime;
      time_t mtime;
      FileMode mode;
      uint32_t Uid;
      uint32_t Gid;
  };
}
