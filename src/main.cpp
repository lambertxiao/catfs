#include <iostream>
#include "meta/meta.h"
#include "fs/fs.h"

#define FUSE_USE_VERSION 34
#include "libfuse/fuse.h"

using namespace catfs::fs;

int main(int argc, char **argv)
{
  std::cout << "start catfs" << std::endl;

  CatFs *cfs = new CatFs();
  struct fuse_operations cfs_oper;
  cfs_oper.getattr = cfs->getattr;

  int fuse_res = fuse_main(0, NULL, &cfs_oper, NULL);
  if (fuse_res == 0)
  {
    std::cout << "fuse res" << std::endl;
  }
  return 0;
}
