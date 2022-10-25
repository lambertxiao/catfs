#include <memory>
#include "meta/meta.h"
#include "storage/storage.h"

namespace catfs {
  namespace meta {
    struct MetaOpt {};

    class MetaImpl : Meta {
    public:
      MetaImpl(MetaOpt opt, std::shared_ptr<LocalMeta> lm, std::shared_ptr<stor::Stor> stor) {
        this->opt = opt;
        this->stor = stor;
        this->lm = lm;
      }

    private:
      MetaOpt opt;
      std::shared_ptr<LocalMeta> lm;
      std::shared_ptr<stor::Stor> stor;
    };
  }
}
