#ifndef CATFS_ERRORS_H_
#define CATFS_ERRORS_H_
#include <stdint.h>
#include <stdexcept>

#include <types/inode.h>

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

    class ERR_SERVER_ERROR : public std::logic_error
    {
    public:
      ERR_SERVER_ERROR(std::string msg) : logic_error(msg) {}
    };

    class ERR_ENOTEMPTY : public std::logic_error
    {
    public:
      ERR_ENOTEMPTY() : logic_error("directory not empty") {}
    };

    class ERR_ENOENT : public std::logic_error
    {
    public:
      ERR_ENOENT() : logic_error("no such file or directory") {}
    };

    class ERR_ENOTDIR : public std::logic_error
    {
    public:
      ERR_ENOTDIR() : logic_error("not a directory") {}
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
