#ifndef CATFS_FS_OPENFILE_H_
#define CATFS_FS_OPENFILE_H_

#include <string>
#include <memory>
#include <condition_variable>
#include "types/inode.h"
#include "types/handle.h"
#include "types/file.h"
#include "fs/freader/freader.h"
#include "fs/fwriter/fwriter.h"
#include "stor/stor.h"

namespace catfs
{
  namespace fs
  {
    class OpenFile
    {
    private:
      FReader reader;
      FWriter writer;
      bool hasWrote;
      bool isWriting;
      int32_t firstWrite;
      std::condition_variable writeDoneCond; // 用来通知读端写入已经结束
      std::shared_ptr<stor::Stor> stor;

    public:
      std::string name;
      types::InodeID ino;
      types::InodeID pino;
      types::HandleID hno;
      types::File file;

      OpenFile(types::InodeID ino, types::HandleID hno)
      {
        this->ino = ino;
        this->hno = hno;
      }

      int read(off_t off, size_t size, char* buf);
    };
  }
}

#endif
