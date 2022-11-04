#ifndef CATFS_FTREENODE_H_
#define CATFS_FTREENODE_H_

#include <string>
#include <unordered_map>

#include "types/obj.h"

namespace catfs
{
  namespace types
  {
    struct FTreeNode
    {
      std::string name;
      bool is_dir;
      std::unordered_map<std::string, types::FTreeNode> children;
      types::ObjInfo oinfo;
    };
  }
}
#endif
