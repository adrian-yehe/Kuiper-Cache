#ifndef __PARAMS__QueuedPrefetcher__
#define __PARAMS__QueuedPrefetcher__
#include <cstdint>
#include "BasePrefetcher.h"

namespace Kuiper {
    namespace Cache {
        struct QueuedPrefetcherParams
            : public BasePrefetcherParams
        {
            bool cache_snoop;
            int latency;
            int max_prefetch_requests_with_pending_translation;
            bool queue_filter;
            int queue_size;
            bool queue_squash;
            bool tag_prefetch;
            int throttle_control_percentage;
        };
    }
}

#endif // __PARAMS__QueuedPrefetcher__
