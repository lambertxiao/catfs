#define FUSE_USE_VERSION 34
#include "libfuse/fuse.h"

namespace catfs
{
  namespace fs
  {
    class CatFs
    {
    public:
      int (*getattr)(const char *, struct stat *, struct fuse_file_info *fi);
    };
  }
}