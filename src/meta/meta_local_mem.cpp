#include <fcntl.h>
#include <time.h>
#include <functional>
#include <queue>

#include "fmtlog/fmtlog.h"
#include "meta/meta_local_mem.h"
#include "types/inode.h"
#include "util/time.h"

#define rlock_dentry_idx std::shared_lock lock(m_locks[ino % LOCK_COUNT]);

#define wlock_dentry_idx std::unique_lock lock(m_locks[ino % LOCK_COUNT]);

namespace catfs {
namespace meta {
LocalMemMeta::LocalMemMeta(MetaOpt &opt) {
  this->opt = opt;
  this->next_inode_id = types::ROOT_INODE_ID;

  auto inode = new types::Inode();
  inode->ino = this->get_next_inode_id();
  inode->mode = S_IFDIR | 0766;
  inode->uid = opt.uid;
  inode->gid = opt.uid;

  timespec ts;
  timespec_get(&ts, TIME_UTC);
  inode->ctime = ts;
  inode->mtime = ts;

  this->root_dentry = new types::Dentry("", inode);
  this->save_dentry_index(this->root_dentry);
}

void LocalMemMeta::save_dentry_index(types::Dentry *d) {
  auto ino = d->inode->ino;
  wlock_dentry_idx

      dentry_index[ino % LOCK_COUNT][ino] = d;
}

types::Inode *LocalMemMeta::get_inode(types::InodeID ino) {
  rlock_dentry_idx

      auto dentry = dentry_index[ino % LOCK_COUNT][ino];

  if (dentry == NULL) {
    return NULL;
  }

  return dentry->inode;
}

Inode *LocalMemMeta::update_inode(InodeID ino, InodeUpdateAttr &updater, bool sync) {
  auto inode = get_inode(ino);
  if (inode == NULL) {
    throw types::InvalidInodeID(ino);
  }

  if (updater.size != NULL) {
    inode->size = *updater.size;
  }
  if (updater.mode != NULL) {
    inode->mode = *updater.mode;
  }
  if (updater.uid != NULL) {
    inode->uid = *updater.uid;
  }
  if (updater.gid != NULL) {
    inode->gid = *updater.gid;
  }

  return inode;
}

void LocalMemMeta::remove_inode(InodeID ino) { wlock_dentry_idx dentry_index[ino % LOCK_COUNT].erase(ino); }

Dentry *LocalMemMeta::get_dentry(InodeID ino) { rlock_dentry_idx return dentry_index[ino % LOCK_COUNT][ino]; }

Dentry *LocalMemMeta::create_dentry(InodeID pino, std::string name, Inode *inode) {
  auto parent = get_dentry(pino);
  if (parent == NULL) {
    throw types::InvalidInodeID(pino);
  }

  auto target = add_child_for_dentry(parent, name, inode);
  return target;
}

Dentry *LocalMemMeta::add_child_for_dentry(Dentry *parent, std::string name, Inode *inode) {
  auto child = parent->add_child(name, inode);
  this->save_dentry_index(child);
  return child;
}

Dentry *LocalMemMeta::find_dentry(InodeID pino, std::string name) {
  auto parent = get_dentry(pino);
  if (parent == NULL) {
    throw types::InvalidInodeID(pino);
  }

  return parent->get_child(name);
}

Dentry *LocalMemMeta::create_dentry_from_obj(InodeID pino, std::string name, types::ObjInfo obj, bool is_dir) {
  auto parent = get_dentry(pino);
  if (parent == NULL) {
    throw types::InvalidInodeID(pino);
  }

  auto inode = obj2inode(obj, is_dir);
  return add_child_for_dentry(parent, name, inode);
}

Inode *LocalMemMeta::obj2inode(types::ObjInfo &obj, bool is_dir) {
  auto inode = new Inode();
  inode->ino = get_next_inode_id();
  inode->size = obj.size;

  if (is_dir) {
    inode->mtime = util::now();
    inode->ctime = util::now();
  }

  if (obj.uid != NULL)
    inode->uid = *obj.uid;
  else
    inode->uid = opt.uid;

  if (obj.gid != NULL)
    inode->gid = *obj.gid;
  else
    inode->gid = opt.gid;

  if (is_dir)
    inode->mode = S_IFDIR | 0766;
  else
    inode->mode = S_IFREG | 0644;

  if (obj.mode != NULL) {
    auto perm = 0777;
    inode->mode = (inode->mode & ~perm) | (*obj.mode & perm);
  }

  return inode;
}

void LocalMemMeta::remove_dentry(InodeID pino, std::string name) {
  auto parent = get_dentry(pino);
  if (parent == NULL) {
    throw types::InvalidInodeID(pino);
  }

  // 这里只将节点从parent上移除，不释放inode，因为可能有正在进行的write
  // 在releaseFileHandle的时候会check
  // inode是否还在parent上，不存在了代表被删掉了，则释放inode自身
  parent->remove_child(name);
}

void LocalMemMeta::rename(InodeID src_pino, std::string src_name, InodeID dst_pino, std::string dst_name) {
  auto src_parent = get_dentry(src_pino);
  if (src_parent == NULL) {
    throw types::InvalidInodeID(src_pino);
  }

  auto dst_parent = get_dentry(dst_pino);
  if (dst_parent == NULL) {
    throw types::InvalidInodeID(dst_pino);
  }

  auto src_dentry = src_parent->get_child(src_name);
  if (src_dentry == NULL) return;

  src_parent->remove_child(src_name);
  add_child_for_dentry(dst_parent, dst_name, src_dentry->inode);
}

Inode *LocalMemMeta::create_new_inode(mode_t mode, uint32_t gid, uint32_t uid) {
  auto inode = new Inode();
  inode->ino = get_next_inode_id();
  inode->mode = mode;
  inode->gid = gid;
  inode->uid = uid;
  inode->ctime = util::now();
  inode->mtime = util::now();

  return inode;
}

InodeID LocalMemMeta::get_next_inode_id() {
  std::unique_lock lock(next_inode_id_locker);
  auto ino = next_inode_id;
  next_inode_id++;
  return ino;
}

void LocalMemMeta::build_dentries(InodeID pino, types::FTreeNode &root) {
  std::function<void(Dentry * parent, std::unordered_map<string, types::FTreeNode> & children)> build;

  build = [this, &build](Dentry *parent, std::unordered_map<string, types::FTreeNode> &children) {
    parent->synced = true;
    for (auto &[_, child] : children) {
      auto child_dentry = parent->get_child(child.name);

      if (child_dentry == NULL) {
        logd("dentry add child, name:{}, is_dir:{}", child.name, child.is_dir);
        child_dentry = create_dentry_from_obj(parent->inode->ino, child.name, child.oinfo, child.is_dir);
      }
      else {
        child_dentry->update(child.oinfo.size, child.oinfo.ctime, child.oinfo.mtime);
      }

      if (child.is_dir) build(child_dentry, child.children);

      child_dentry->synced = true;
    }
  };

  auto parent = get_dentry(pino);
  build(parent, root.children);
};

void LocalMemMeta::clear_unsync_dentry(Dentry &parent) {
  std::queue<Dentry *> q;
  q.push(&parent);

  while (!q.empty()) {
    auto d = q.front();
    q.pop();

    d->synced = true;
    d->inc_ttl(opt.dcache_timeout);

    if (d->child_count() != 0) {
      d->complete();
    }

    for (auto &[_, dentry] : d->children) {
      auto dname = dentry->name;
      if (!dentry->synced) {
        d->remove_child(dname);
        logd("ino:{} remove missing dentry:{}", d->inode->ino, dname);
        continue;
      }

      dentry->synced = false;

      if (dentry->is_dir()) {
        q.push(dentry);
        dentry->inc_ttl(opt.dcache_timeout);

        if (dentry->child_count() != 0) {
          dentry->complete();
        }
      } else {
        dentry->inc_ttl(opt.dcache_timeout);
      }
    }
  }
}
}  // namespace meta
}  // namespace catfs
