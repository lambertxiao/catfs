#ifndef CATFS_TYPES_DENTRY_H_
#define CATFS_TYPES_DENTRY_H_
#include <fcntl.h>
#include <time.h>
#include <ctime>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "fmtlog/fmtlog.h"
#include "types/dirent.h"
#include "util/time.h"

namespace catfs {
namespace types {
const uint32_t D_COMPLETE = 1;

class Dentry {
 public:
  std::shared_mutex mutex;
  std::string name;
  Dentry *parent = NULL;
  Inode *inode = NULL;
  timespec ttl;
  uint32_t flags = 0;
  bool synced = false;
  std::unordered_map<std::string, Dentry *> children;

  Dentry(std::string name, Inode *inode) {
    this->name = name;
    this->inode = inode;
    ttl.tv_sec = 0;
    ttl.tv_nsec = 0;
  }

  ~Dentry() {
    logw("析构dentry name:{}", name);
    delete inode;
  }

  bool is_root() { return this->inode->ino == ROOT_INODE_ID; }

  std::string get_full_path() {
    if (is_root()) return "";

    auto fullpath = this->name;
    auto parent = this->parent;

    while (parent != NULL && !parent->is_root()) {
      fullpath = parent->name + "/" + fullpath;
      parent = parent->parent;
    }

    return fullpath;
  }

  std::string get_full_path_with_slash() {
    if (is_root()) return "";
    return get_full_path() + "/";
  }

  Dentry *get_child(std::string name) {
    std::shared_lock lock(mutex);

    if (children.count(name) > 0) {
      return children[name];
    }
    return NULL;
  }

  Dentry *add_child(std::string name, Inode *inode) {
    std::unique_lock lock(mutex);

    auto child = new Dentry(name, inode);
    child->parent = this;
    children[name] = child;

    return child;
  }

  void remove_child(std::string name) {
    std::unique_lock lock(mutex);

    if (children.count(name) > 0) {
      auto target = children[name];
      free(target->inode);
      free(target);
      children.erase(name);
    }
  }

  bool is_expired() {
    auto now = util::now();

    if (ttl.tv_sec == now.tv_sec) {
      return ttl.tv_nsec < now.tv_nsec;
    } else {
      return ttl.tv_sec < now.tv_sec;
    }
  }

  bool is_complete() { return (this->flags & D_COMPLETE) == D_COMPLETE; }

  void complete() { this->flags |= D_COMPLETE; }

  void clear_complete() { this->flags &= (~D_COMPLETE); }

  bool is_dir() { return (this->inode->mode & S_IFDIR) != 0; }

  void set_ttl(timespec ttl) { this->ttl = ttl; }

  void inc_ttl(uint32_t duration) {
    auto tc = util::now();
    // timespec tc;
    // timespec_get(&tc, TIME_UTC);
    tc.tv_sec += duration;
    this->ttl = tc;
  }

  void update(uint64_t size, timespec ctime, timespec mtime) {
    this->inode->size = size;
    this->inode->ctime = ctime;
    this->inode->mtime = mtime;
  }

  void children_list(std::vector<types::Dirent> &dirents) {
    std::shared_lock lock(mutex);

    for (auto &[k, v] : this->children) {
      dirents.push_back({name : v->name, inode : v->inode});
    }
  }

  int child_count() {
    std::shared_lock lock(mutex);
    return this->children.size();
  }
};
}
}
#endif
