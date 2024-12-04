#include "include/prefetch_tagged.h"
#include "params/TaggedPrefetcher.h"

namespace Kuiper {
    namespace Cache {
        namespace prefetch {
            Tagged::Tagged(const TaggedPrefetcherParams &p)
                : Queued(p), degree(p.degree)
            {}

            void
            Tagged::calculatePrefetch(const PrefetchInfo &pfi,
                                      std::vector<AddrPriority> &addresses,
                                      const CacheAccessor &cache)
            {
                Addr blkAddr = blockAddress(pfi.getAddr());

                for (int d = 1; d <= degree; d++)
                {
                    Addr newAddr = blkAddr + d * (blkSize);
                    addresses.push_back(AddrPriority(newAddr, 0));
                }
            }
        } // namespace prefetch
    } // namespace Cache
} // namespace Kuiper
