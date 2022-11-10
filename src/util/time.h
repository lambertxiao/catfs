#ifndef CATFS_UTIL_TIME_H_
#define CATFS_UTIL_TIME_H_

#include <time.h>

namespace catfs {
namespace util {
static timespec now() {
  timespec tc;
  timespec_get(&tc, TIME_UTC);
  return tc;
}
}  // namespace util
}  // namespace catfs

#endif
