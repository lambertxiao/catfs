#include "meta/meta.h"
#include "meta/meta_impl.h"
#include "types/inode.h"

namespace catfs {
  namespace meta {
    USING_TYPES

    Inode* MetaImpl::get_inode(InodeID ino) {
      return lm.get()->get_inode(ino);
    };

    Dentry* MetaImpl::find_dentry(InodeID pino, std::string name, bool onlyLocal) {
      auto parent = lm.get()->find_dentry(pino, name);

      // dentry, err := m.local.FindDentry(pino, name)
      // if err != nil {
      //   return nil, err
      // }

      // if dentry == nil {
      //   if onlyLocal {
      //     return nil, types.ENOENT
      //   }

      //   parent := m.local.GetDentry(pino)
      //   if parent == nil {
      //     return nil, fmt.Errorf("invalid pino:%d", pino)
      //   }

      //   obj, isDir, err := m.getRemoteObject(parent, name)
      //   if err == nil {
      //     dentry, err := m.local.CreateDentryFromObj(pino, name, *obj, isDir)
      //     if err != nil {
      //       return nil, err
      //     }
      //     dentry.SetTTL(time.Now().Add(m.opt.DCacheTTL))
      //     return dentry, nil
      //   }

      //   if err != types.ENOENT {
      //     return nil, err
      //   }

      //   // 举个例子，如果当前ls的目录为a/b，但服务端存在以a/b/*为前缀的key，需要将a/b的目录在本地创建出来
      //   dirpath := parent.GetFullPathWithSlash() + name
      //   exist, err := m.isRemoteDirExist(dirpath)
      //   if err != nil {
      //     return nil, err
      //   }

      //   if !exist {
      //     return nil, types.ENOENT
      //   }

      //   // add virtual dir in local
      //   dentry = parent.AddChild(name, m.local.CreateNewInode(parent.Inode.Mode, parent.Inode.Gid, parent.Inode.Uid))
      //   dentry.SetTTL(time.Now().Add(m.opt.DCacheTTL))
      //   return dentry, nil
      // } else {
      //   if !dentry.IsExpired() {
      //     return dentry, nil
      //   }

      //   var path string
      //   if dentry.IsDir() {
      //     path = dentry.GetFullPathWithSlash()
      //   } else {
      //     path = dentry.GetFullPath()
      //   }

      //   logg.Dlog.Debugf("pino:%d fpath:%s is expired", pino, path)
      //   reply, err := m.storage.HeadFile(&storage.HeadFileRequest{Key: path})
      //   if err != nil {
      //     if err == types.ENOENT && dentry.IsDir() {
      //       // local存的dentry可能是virtual dir, 服务端不存在对应的key
      //       dentry.SetTTL(time.Now().Add(m.opt.DCacheTTL))
      //       return dentry, nil
      //     }
      //     return nil, err
      //   }

      //   obj := reply.Info
      //   var mode os.FileMode
      //   if obj.Mode != nil {
      //     mode = (os.FileMode(*obj.Mode) & os.ModePerm)
      //   } else {
      //     if dentry.IsDir() {
      //       mode = os.ModeDir | 0755
      //     } else {
      //       mode = 0644
      //     }
      //   }

      //   update := InodeUpdateAttr{
      //     Size: &obj.Size, Uid: obj.Uid, Gid: obj.Gid, Mode: &mode,
      //   }

      //   _, err = m.local.UpdateInode(dentry.Inode.Ino, update, true)
      //   if err != nil {
      //     return nil, err
      //   }
      //   dentry.SetTTL(time.Now().Add(m.opt.DCacheTTL))
      //   return dentry, nil
      // }
      return NULL;
    }
  }
}