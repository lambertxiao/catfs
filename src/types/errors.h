#ifndef CATFS_ERRORS_H_
#define CATFS_ERRORS_H_
#include <stdint.h>
#include <stdexcept>

namespace catfs
{
  namespace types
  {
    using ErrCode = uint32_t;

    class InvalidInodeID : public std::logic_error
    {
    private:
      InodeID ino;

    public:
      InvalidInodeID(InodeID ino) : logic_error("Invaild ino")
      {
        this->ino = ino;
      }
    };

  } // namespace types
} // namespace catfs
#endif
