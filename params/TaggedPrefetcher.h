#ifndef __TAGGED_PREFETCHER_H__
#define __TAGGED_PREFETCHER_H__

#include <cstdint>
#include "QueuedPrefetcher.h"

namespace Kuiper {
    namespace Cache {
        struct TaggedPrefetcherParams
            : public QueuedPrefetcherParams {
            Kuiper::Cache::prefetch::Tagged *tagged;
            int degree;
        };
    }
}

#endif /* __TAGGED_PREFETCHER_H__ */
