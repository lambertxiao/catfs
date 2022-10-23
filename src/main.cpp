#include <iostream>
#include "meta/meta.h"
#include "fs/fuse.h"
#include "fs/fs.h"
#include "fs/fsa.h"

using namespace catfs::fs;

int main(int argc, char **argv)
{
  std::cout << "start catfs" << std::endl;
  struct fuse_operations cfs_oper;
  cfs_oper.statfs = FuseAdapter::statfs;

  return fuse_main(0, NULL, &cfs_oper, NULL);
}
