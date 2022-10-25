#ifndef CATFS_META_IMPL_H
#define CATFS_META_IMPL_H

#include <memory>
#include "meta/meta.h"
#include "stor/stor.h"

namespace catfs {
  namespace meta {
    struct MetaOpt {};

    class MetaImpl : Meta {
    private: 
      MetaOpt opt;
      std::shared_ptr<LocalMeta> lm;
      std::shared_ptr<stor::Stor> stor;

    public:
      MetaImpl(MetaOpt opt, std::shared_ptr<LocalMeta> lm, std::shared_ptr<stor::Stor> stor) {
        this->opt = opt;
        this->stor = stor;
        this->lm = lm;
      }
    };
  }
}
#endif
