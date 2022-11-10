#include "fs/fs.h"
#include "fs/open_dir.h"

namespace catfs
{
  namespace fs
  {
    const Inode *CatFS::lookup_inode(InodeID ino)
    {
      return meta->get_inode(ino);
    }

    const Dentry *CatFS::find_dentry(InodeID pino, std::string name)
    {
      return meta->find_dentry(pino, name, false);
    }

    const Dentry *CatFS::create_dentry(InodeID pino, std::string name, mode_t mode)
    {
      return meta->create_dentry(pino, name, mode);
    }

    void CatFS::remove_dentry(InodeID parent, std::string name)
    {
      meta->remove_dentry(parent, name);
    }

    types::HandleID CatFS::get_next_handle_id()
    {
      std::lock_guard lock(next_handle_id_lock);
      auto hno = next_handle_id;
      next_handle_id++;
      return hno;
    }
  }
}
