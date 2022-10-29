#include <map>
#include "meta/meta.h"
#include "meta/meta_impl.h"
#include "types/inode.h"
#include "stor/stor.h"
#include "util/time.h"

namespace catfs
{
  namespace meta
  {
    USING_TYPES

    Inode *MetaImpl::get_inode(InodeID ino)
    {
      return local_meta.get()->get_inode(ino);
    };

    stor::ObjInfo *MetaImpl::get_remote_obj(Dentry &parent, std::string name)
    {
      // 1. 检查是否有同名的文件
      // 2. 检查是否有同名的目录
      auto fullname = parent.get_full_path_with_slash() + name;
      return check_dentry_exist(fullname);
    }

    stor::ObjInfo *MetaImpl::check_dentry_exist(std::string path)
    {
      auto req = stor::HeadFileReq{obj_key : path};
      auto resp = stor->head_file(&req);

      if (resp->obj == NULL)
      {
        auto req = stor::HeadFileReq{obj_key : path + "/"};
        auto resp = stor->head_file(&req);

        if (resp != NULL)
        {
          return resp->obj;
        }
      }

      return resp->obj;
    }

    Dentry *MetaImpl::find_dentry(InodeID pino, std::string name, bool onlyLocal)
    {
      auto dentry = local_meta.get()->find_dentry(pino, name);
      if (dentry == NULL)
      {
        if (onlyLocal)
        {
          return NULL;
        }

        auto parent = local_meta->get_dentry(pino);
        if (parent == NULL)
        {
          throw types::InvalidInodeID(pino);
        }

        auto obj = get_remote_obj(*parent, name);
        if (obj != NULL)
        {
          dentry = local_meta->create_dentry_from_obj(pino, name, *obj, obj->is_dir);
          dentry->inc_ttl(opt.dcache_timeout);
          return dentry;
        }

        // 举个例子，如果当前ls的目录为a/b，但服务端存在以a/b/*为前缀的key，需要将a/b的目录在本地创建出来
        auto prefix = parent->get_full_path_with_slash() + name;
        auto exist = is_remote_dir_exist(prefix);
        if (!exist)
        {
          return NULL;
        }

        // add virtual dir in local
        dentry = parent->add_child(name, local_meta->create_new_inode(parent->inode->mode, parent->inode->gid, parent->inode->uid));
        dentry->inc_ttl(opt.dcache_timeout);
        return dentry;
      }
      else
      {
        if (!dentry->is_expired())
        {
          return dentry;
        }

        std::string path;
        if (dentry->is_dir())
        {
          path = dentry->get_full_path_with_slash();
        }
        else
        {
          path = dentry->get_full_path();
        }

        auto req = stor::HeadFileReq{obj_key : path};
        auto reply = stor->head_file(&req);

        if (reply->obj == NULL && dentry->is_dir())
        {
          // local存的dentry可能是virtual dir, 服务端不存在对应的key
          dentry->inc_ttl(opt.dcache_timeout);
          return dentry;
        }

        auto obj = reply->obj;
        mode_t mode;
        if (obj->mode != NULL)
        {
          mode = *obj->mode & 0777;
        }
        else
        {
          if (dentry->is_dir())
          {
            mode = S_IFDIR | 0755;
          }
          else
          {
            mode = 0644;
          }
        }

        auto updater = InodeUpdateAttr{
          size : &obj->size,
          mode : &mode,
          uid : obj->uid,
          gid : obj->gid,
        };

        local_meta->update_inode(dentry->inode->ino, updater, true);
        dentry->inc_ttl(opt.dcache_timeout);
        return dentry;
      }
    }

    bool MetaImpl::is_remote_dir_exist(std::string path)
    {
      auto req = stor::ListObjectsReq{
        delimiter : "",
        prefix : path,
        max : 1,
      };
      auto reply = stor->list_objects(&req);
      return reply->objs.size() > 0;
    }

    Dentry *MetaImpl::create_dentry(InodeID pino, std::string name, mode_t mode)
    {
      auto dentry = find_dentry(pino, name, false);
      if (dentry != NULL)
      {
        return dentry;
      }

      auto parent = local_meta->get_dentry(pino);

      std::string obj_key;
      if (parent->is_root())
      {
        obj_key = name;
      }
      else
      {
        obj_key = parent->get_full_path() + "/" + name;
      }

      if ((mode & S_IFDIR) != 0)
      {
        obj_key += "/";
      }

      std::map<string, string> meta_data;
      meta_data["mode"] = mode;
      meta_data["uid"] = parent->inode->uid;
      meta_data["gid"] = parent->inode->gid;

      auto req = stor::PutFileReq{
        obj_key : obj_key,
        meta_data : meta_data,
      };

      stor->put_file(&req);
      auto inode = local_meta->create_new_inode(mode, parent->inode->gid, parent->inode->uid);
      dentry = local_meta->create_dentry(pino, name, inode);

      return dentry;
    }

    void MetaImpl::remove_dentry(InodeID pino, std::string name)
    {
      auto dentry = find_dentry(pino, name, false);
      if (dentry == NULL)
      {
        return;
      }

      auto fullpath = dentry->get_full_path();
      if (!dentry->is_dir())
      {
        auto req = stor::DeleteFileReq{obj_key : fullpath};
        stor->delete_file(&req);
      }
      else
      {
        if (dentry->child_count() != 0)
        {
          throw types::ERR_ENOTEMPTY();
        }

        auto req = stor::DeleteFileReq{obj_key : fullpath + "/"};
        stor->delete_file(&req);
      }

      local_meta->remove_dentry(pino, name);
    }

    Dentry* MetaImpl::get_dentry(InodeID ino)
    {
      return local_meta->get_dentry(ino);
    }

    std::vector<Dirent> MetaImpl::load_sub_dentries(InodeID ino) 
    {
      std::vector<Dirent> dirents;
      auto dentry = local_meta->get_dentry(ino);
      if (dentry == NULL)
      {
        throw types::ERR_ENOENT();
      }

      dirents.push_back(Dirent{name: ".", inode: dentry->inode});
      dirents.push_back(Dirent{name: "..", inode: dentry->inode});

      if (!dentry->is_complete() || dentry->is_expired())
      {
        refresh_sub_dentries(*dentry, false);
      }

      for (auto &d: dentry->children_list())
      {
        dirents.push_back(Dirent{name: d->name, inode: d->inode});
      }

      return dirents;
    } 

    void MetaImpl::refresh_sub_dentries(Dentry& dentry, bool recursive)
    {
      auto prefix = dentry.get_full_path_with_slash();
    }
  }
}
