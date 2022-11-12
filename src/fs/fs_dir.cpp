#include "fs/fs.h"
#include "fs/open_dir.h"
#include "types/handle.h"

namespace catfs {
namespace fs {
types::HandleID CatFS::opendir(InodeID ino) {
  auto dentry = meta->get_dentry(ino);
  if (dentry == NULL) throw types::ERR_ENOENT();

  auto hno = get_next_handle_id();
  auto open_dir = new OpenDir(dentry->inode->ino, hno);

  open_dir_lock.lock();
  open_dirs[hno] = open_dir;
  open_dir_lock.unlock();

  return hno;
}

std::vector<Dirent> CatFS::read_dir(types::HandleID hno, off_t off, size_t size) {
  open_dir_lock.lock();
  auto open_dir = open_dirs[hno];
  open_dir_lock.unlock();

  if (open_dir == NULL) throw "hno is not valid";

  if (open_dir->dentries.empty()) meta->load_sub_dentries(open_dir->ino, open_dir->dentries);

  auto begin = open_dir->dentries.begin() + off;
  std::vector<Dirent> ret;

  if (off + size >= open_dir->dentries.size()) {
    ret.assign(begin, open_dir->dentries.end());
  } else {
    auto end = open_dir->dentries.begin() + off + size;
    ret.assign(begin, end);
  }

  return ret;
}

void CatFS::release_dir(InodeID ino, types::HandleID hno) {
  open_dir_lock.lock();
  open_dirs.erase(hno);
  open_dir_lock.unlock();
}
}  // namespace fs
}  // namespace catfs
