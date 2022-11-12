#include "fs/fs.h"
#include "fs/open_file.h"
#include "fs/freader/freader_direct.h"
#include "fs/fwriter/fwriter_seq.h"
#include "types/rtfile.h"

namespace catfs {
namespace fs {
types::HandleID CatFS::openfile(InodeID ino, int flags) {
  auto dentry = meta->get_dentry(ino);
  if (dentry == NULL) throw types::ERR_ENOENT();

  auto hno = get_next_handle_id();
  auto file_ptr = std::make_shared<types::RTFile>(dentry->get_full_path(), dentry->inode->size);
  auto of = new OpenFile(dentry->inode->ino, hno, file_ptr);

  // todo 判断flags和挂在参数
  of->reader = new DirectReader(file_ptr);
  of->reader->set_stor(this->stor);
  of->writer = new SequenceWriter(file_ptr);
  of->writer->set_stor(this->stor);

  open_file_lock.lock();
  open_files[hno] = of;
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

int CatFS::writefile(HandleID hno, off_t off, size_t size, const char *buf) {
  open_file_lock.lock_shared();
  auto of = open_files[hno];
  open_file_lock.unlock_shared();

  if (of == NULL) {
    loge("hno:{} no such open_file", hno);
    throw ENOENT;
  }

  of->write(off, size, buf);

  if (of->has_wrote) {
    meta->update_inode_size(of->ino, of->rtfile->size, false);
  }

  return size;
}

void CatFS::release_file(HandleID hno) {
  open_file_lock.lock_shared();
  auto of = open_files[hno];
  open_file_lock.unlock_shared();

  if (of == NULL) {
    loge("hno:{} no such open_file", hno);
    throw ENOENT; 
  }

  of->release();
}

}  // namespace fs
}  // namespace catfs
