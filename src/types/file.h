#include <string>

namespace catfs
{
  namespace types
  {
    class File {
    public:
      std::string path;
      uint64_t size;
    };
  }
}
