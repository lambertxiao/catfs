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

    HandleID CatFS::get_next_handle_id()
    {
      std::lock_guard lock(next_handle_id_lock);
      auto hno = next_handle_id;
      next_handle_id++;
      return hno;
    }

    HandleID CatFS::opendir(InodeID ino)
    {
      auto dentry = meta->get_dentry(ino);
      if (dentry == NULL)
      {
        throw types::ERR_ENOENT();
      }

      auto hno = get_next_handle_id();
      auto open_dir = new OpenDir(dentry->inode->ino, hno);

      open_dir_lock.lock();
      open_dirs[hno] = open_dir;
      open_dir_lock.unlock();

      return hno;
    }

    std::vector<Dirent> CatFS::read_dir(HandleID hno, off_t off, size_t size)
    {
      open_dir_lock.lock();
      auto open_dir = open_dirs[hno];
      open_dir_lock.unlock();

      if (open_dir == NULL)
        throw "hno is not valid";

      if (open_dir->dentries.empty())
        open_dir->dentries = meta->load_sub_dentries(open_dir->ino);

      if (off != 0) off++;

      auto begin = open_dir->dentries.begin() + off;
      std::vector<Dirent> ret;

      if (off+size >= open_dir->dentries.size())
      {
        ret.assign(begin, open_dir->dentries.end());
      }
      else
      {
        auto end = open_dir->dentries.begin() + off + size;
        ret.assign(begin, end);
      }

      return ret;
    }

    void CatFS::release_dir(InodeID ino, HandleID hno)
    {
      open_dir_lock.lock();
      open_dirs.erase(hno);
      open_dir_lock.unlock();
    }
  }
}
