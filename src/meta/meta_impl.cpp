#include "meta/meta.h"
#include "meta/meta_impl.h"
#include "types/inode.h"
#include "stor/stor.h"
#include "util/time.h"

namespace catfs {
  namespace meta {
    USING_TYPES

    Inode* MetaImpl::get_inode(InodeID ino) {
      return local_meta.get()->get_inode(ino);
    };

    stor::ObjInfo* MetaImpl::get_remote_obj(Dentry& parent, std::string name) {
      // 1. 检查是否有同名的文件
      // 2. 检查是否有同名的目录
      auto fullname = parent.get_full_path_with_slash() + name;
      return check_dentry_exist(fullname);
    }

    stor::ObjInfo* MetaImpl::check_dentry_exist(std::string path) {
      auto req = stor::HeadFileReq{obj_key: path};
      auto resp = stor->head_file(&req);
      
      if (resp->obj == NULL) {
        auto req = stor::HeadFileReq{obj_key: path+ "/"};
        auto resp = stor->head_file(&req);

        if (resp != NULL) {
          return resp->obj;
        }
      }

      return resp->obj;
    }

    Dentry* MetaImpl::find_dentry(InodeID pino, std::string name, bool onlyLocal) {
      auto dentry = local_meta.get()->find_dentry(pino, name);
      if (dentry == NULL) {
        if (onlyLocal) {
          return NULL;
        }

        auto parent = local_meta->get_dentry(pino);
        if (parent == NULL) {
          throw types::InvalidInodeID(pino);
        }

        auto obj = get_remote_obj(*parent, name);
        if (obj != NULL) {
          dentry = local_meta->create_dentry_from_obj(pino, name, *obj, obj->is_dir);
          dentry->inc_ttl(opt.dcache_timeout);
          return dentry;
        }

        // 举个例子，如果当前ls的目录为a/b，但服务端存在以a/b/*为前缀的key，需要将a/b的目录在本地创建出来
        auto prefix = parent->get_full_path_with_slash() + name;
        auto exist = is_remote_dir_exist(prefix);
        if (!exist) {
          return NULL;
        }

        // add virtual dir in local
        dentry = parent->add_child(name, local_meta->create_new_inode(parent->inode->mode, parent->inode->gid, parent->inode->uid));
        dentry->inc_ttl(opt.dcache_timeout);
        return dentry;
      } else {
        if (!dentry->is_expired()) {
          return dentry;
        }

        std::string path;
        if (dentry->is_dir()) {
          path = dentry->get_full_path_with_slash();
        } else {
          path = dentry->get_full_path();
        }

        auto req = stor::HeadFileReq{obj_key: path};
        auto reply = stor->head_file(&req);

        if (reply->obj == NULL && dentry->is_dir()) {
          // local存的dentry可能是virtual dir, 服务端不存在对应的key
          dentry->inc_ttl(opt.dcache_timeout);
          return dentry;
        } 

        auto obj = reply->obj;
        mode_t mode;
        if (obj->mode != NULL) {
          mode = *obj->mode & 0777;
        } else {
          if (dentry->is_dir()) {
            mode = S_IFDIR | 0755;
          } else {
            mode = 0644;
          }
        }

        auto updater = InodeUpdateAttr{
          size: &obj->size,
          mode: &mode,
          uid: obj->uid,
          gid: obj->gid,
        };

        local_meta->update_inode(dentry->inode->ino, updater, true);
        dentry->inc_ttl(opt.dcache_timeout);
        return dentry;
      }
    }

    bool MetaImpl::is_remote_dir_exist(std::string path) {
      auto req = stor::ListObjectsReq{
        delimiter: "",
		    prefix:    path,
		    max:       1,
      };
      auto reply = stor->list_objects(&req);
      return reply->objs.size() > 0;
    }
  }
}