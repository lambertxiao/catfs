#ifndef CATFS_TYPES_OBJ_H_
#define CATFS_TYPES_OBJ_H_

#include <string>

namespace catfs {
namespace types {
struct ObjInfo {
  std::string name;
  uint64_t size;
  timespec ctime;
  timespec mtime;
  uint32_t *uid = NULL;
  uint32_t *gid = NULL;
  mode_t *mode = NULL;
};
}  // namespace types
}  // namespace catfs
#endif
