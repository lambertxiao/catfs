#ifndef CATFS_META_IMPL_H_
#define CATFS_META_IMPL_H_

#include <memory>
#include <ctime>
#include "meta/meta.h"
#include "meta/meta_local.h"
#include "stor/stor.h"

namespace catfs {
  namespace meta {
    
    class MetaImpl : public Meta {
    private:
      MetaOpt opt;
      std::shared_ptr<LocalMeta> lm;
      std::shared_ptr<stor::Stor> stor;

    public:
      MetaImpl(const MetaOpt& opt, std::shared_ptr<LocalMeta> lm, std::shared_ptr<stor::Stor> stor) {
        this->opt = opt;
        this->stor = stor;
        this->lm = lm;
      }
    };
  }
}
#endif
