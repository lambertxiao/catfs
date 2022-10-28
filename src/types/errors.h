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

    class ERR_ENOTEMPTY : public std::logic_error
    {
    public:
      ERR_ENOTEMPTY() : logic_error("directory not empty") {}
    };

    // EEXIST    error = errors.New("file exists")
    // EINVAL    error = errors.New("invalid argument")
    // EIO       error = errors.New("input/output error")
    // ENOATTR   error = errors.New("no data available")
    // ENOENT    error = errors.New("no such file or directory")
    // ENOSYS    error = errors.New("function not implemented")
    // ENOTDIR   error = errors.New("not a directory")
    // EISDIR    error = errors.New("is a directory")
    // ENOTEMPTY error = errors.New("directory not empty")

  } // namespace types
} // namespace catfs
#endif
