#include "fs/freader/freader_direct.h"
#include "fs/fs.h"
#include "fs/open_file.h"
#include "types/rtfile.h"

namespace catfs {
namespace fs {
types::HandleID CatFS::openfile(InodeID ino, int flags) {
  auto dentry = meta->get_dentry(ino);
  if (dentry == NULL) throw types::ERR_ENOENT();

  auto hno = get_next_handle_id();
  auto openfile = new OpenFile(dentry->inode->ino, hno);

  // todo 判断flags

  auto file_ptr = std::make_shared<types::RTFile>(dentry->get_full_path(), dentry->inode->size);
  openfile->reader = new DirectReader(file_ptr);
  openfile->reader->set_stor(this->stor);

  open_file_lock.lock();
  open_files[hno] = openfile;
  open_file_lock.unlock();

  return hno;
}

int CatFS::readfile(HandleID hno, off_t off, size_t size, char *buf) {
  open_file_lock.lock_shared();
  auto of = open_files[hno];
  open_file_lock.unlock_shared();

  if (of == NULL) {
    loge("hno:{} no such open_file", hno);
    throw ENOENT;
  }

  return of->read(off, size, buf);
}
}  // namespace fs
}  // namespace catfs
