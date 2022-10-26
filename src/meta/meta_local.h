#ifndef CATFS_META_LOCAL_H_
#define CATFS_META_LOCAL_H_

#include <string>
#include "meta/meta.h"

namespace catfs {
  namespace meta {
    // local meta is a interface
    class LocalMeta {
    private:
      MetaOpt opt;
      
    public:
      LocalMeta(const MetaOpt& opt) {
        this->opt = opt;
      };
    };
  }
}
#endif
