#ifndef __MEM_CACHE_PREFETCH_TAGGED_HH__
#define __MEM_CACHE_PREFETCH_TAGGED_HH__

#include "queued.h"
#include "cache_packet.h"

namespace Kuiper {
  namespace Cache{
    struct TaggedPrefetcherParams;
    namespace prefetch {

      class Tagged : public Queued {
      protected:
        const int degree;

      public:
        Tagged(const TaggedPrefetcherParams &p);
        ~Tagged() = default;

        void calculatePrefetch(const PrefetchInfo &pfi,
                               std::vector<AddrPriority> &addresses,
                               const CacheAccessor &cache) override;
      };
    }
  } // namespace Cache
} // namespace Kuiper

#endif // __MEM_CACHE_PREFETCH_TAGGED_HH__
