#ifndef __TAGGED_PREFETCHER_H__
#define __TAGGED_PREFETCHER_H__

#include <cstdint>
#include "QueuedPrefetcher.h"
#include "prefetch/include/prefetch_tagged.h"

namespace Kuiper {
    namespace Cache {
        namespace prefetch { 
            class Tagged; 
        };
        struct TaggedPrefetcherParams: 
            public QueuedPrefetcherParams {
            prefetch::Tagged *tagged;
            int degree;
        };
    }
}

#endif /* __TAGGED_PREFETCHER_H__ */
