#ifndef CATFS_FS_FS_H_
#define CATFS_FS_FS_H_

#include <memory>
#include <string>
#include <unordered_map>
#include "mutex"
#include "shared_mutex"

#include "fs/fuse.h"
#include "fs/open_dir.h"
#include "fs/open_file.h"
#include "meta/meta.h"
#include "types/dirent.h"
#include "types/handle.h"
#include "types/inode.h"

namespace catfs {
namespace fs {
using catfs::meta::Meta;
using catfs::types::Dentry;
using catfs::types::Dirent;
using catfs::types::HandleID;
using catfs::types::Inode;
using catfs::types::InodeID;

struct CatFsOpt {};

class CatFS {
 private:
  std::shared_ptr<Meta> meta;
  std::shared_ptr<stor::Stor> stor;
  HandleID next_handle_id = 0;
  std::mutex next_handle_id_lock;
  std::shared_mutex open_dir_lock;
  std::shared_mutex open_file_lock;
  std::unordered_map<HandleID, OpenDir *> open_dirs;
  std::unordered_map<HandleID, OpenFile *> open_files;

 public:
  CatFS(std::shared_ptr<Meta> meta, std::shared_ptr<stor::Stor> stor) {
    this->meta = meta;
    this->stor = stor;
  }

  ~CatFS() {}

  const Inode *lookup_inode(InodeID ino);
  const Dentry *find_dentry(InodeID pino, std::string name);
  const Dentry *create_dentry(InodeID parent, std::string name, mode_t mode);
  void remove_dentry(InodeID parent, std::string name);
  int readfile(HandleID hno, off_t off, size_t size, char *buf);
  int writefile(HandleID hno, off_t off, size_t size, const char *buf);
  void release_file(HandleID hno);
  HandleID opendir(InodeID ino);
  HandleID openfile(InodeID ino, int flags);
  std::vector<Dirent> read_dir(HandleID hno, off_t off, size_t size);
  void release_dir(InodeID ino, HandleID hno);
  HandleID get_next_handle_id();
};
}  // namespace fs
}  // namespace catfs
#endif
