#include <atomic>
#include "fs/open_file.h"
#include "fmtlog/fmtlog.h"

namespace catfs {
namespace fs {

int OpenFile::read(uint64_t off, uint64_t size, char *buf) {
  if (off >= rtfile->size) {
    loge("fpath:{} read offset exceed file size, off:{}, fsize:{}", rtfile->path, off, rtfile->size);
    return 0;
  }

  if (off + size > rtfile->size) {
    size = rtfile->size - off;
  }

  if (size == 0) {
    return 0;
  }

  return reader->read(off, size, buf);
}

int OpenFile::write(uint64_t off, uint64_t size, const char *buf) {
  if (first_write.load() == 0) {
    int expect = 0;
    if (first_write.compare_exchange_strong(expect, 1)) {
      is_writing = true;
    }
  }
  int n = writer->write(off, size, buf);
  has_wrote = true;
  rtfile->size = writer->file_size();
  return n;
}

void OpenFile::release() {
  if (writer != NULL) {
    auto has_excep = false;
    std::exception excep;
    try {
      writer->release();
    } catch(const std::exception& e)
    {
      excep = e;
      has_excep = true;
    }
    
    std::unique_lock<std::mutex> lck(write_done_lock);
    is_writing = false;
    write_done_cond.notify_all();

    if (has_excep) {
      throw excep;
    }
  }

  if (reader != NULL) {
    reader->release();
  }
}

void OpenFile::wait_write_done() {
  if (writer != NULL) {
    std::unique_lock<std::mutex> lk(write_done_lock);

    while (is_writing) {
      write_done_cond.wait(lk);
    }
    // write_done_cond.wait(lk, []{return ready;});
  }

  	// if of.writer != nil {
		// of.writeDoneCond.L.Lock()
		// for of.isWriting {
		// 	of.writeDoneCond.Wait()
		// }
		// of.writeDoneCond.L.Unlock()
}

}  // namespace fs
}  // namespace catfs
