#ifndef CATFS_FS_FS_H_
#define CATFS_FS_FS_H_

#include <string>
#include <memory>
#include <unordered_map>
#include "mutex"
#include "shared_mutex"

#include "fs/fuse.h"
#include "fs/open_dir.h"
#include "fs/open_file.h"
#include "meta/meta.h"
#include "types/inode.h"
#include "types/dirent.h"
#include "types/handle.h"

namespace catfs {
  namespace fs {
    using catfs::types::InodeID;
    using catfs::types::Inode;
    using catfs::types::Dentry;
    using catfs::types::Dirent;
    using catfs::meta::Meta;
    
    struct CatFsOpt {};

    class CatFS {
    private:
      std::shared_ptr<Meta> meta;
      types::HandleID next_handle_id;
      std::mutex next_handle_id_lock;
      std::shared_mutex open_dir_lock;
      std::shared_mutex open_file_lock;
      std::unordered_map<types::HandleID, OpenDir*> open_dirs;
      std::unordered_map<types::HandleID, OpenFile*> open_files;
    
    public:
      CatFS(std::shared_ptr<Meta> meta) {
        this->meta = meta;
      }

      ~CatFS() {}
      
      const Inode* lookup_inode(InodeID ino);
      const Dentry* find_dentry(InodeID pino, std::string name);
      const Dentry* create_dentry(InodeID parent, std::string name, mode_t mode);
      void remove_dentry(InodeID parent, std::string name);

      types::HandleID opendir(InodeID ino);
      types::HandleID openfile(InodeID ino);

      std::vector<Dirent> read_dir(types::HandleID hno, off_t off, size_t size);
      void release_dir(InodeID ino, types::HandleID hno);

      types::HandleID get_next_handle_id();
    };
  }
}
#endif
