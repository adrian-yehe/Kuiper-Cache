#ifndef __PARAMS__BaseTags__
#define __PARAMS__BaseTags__

#include <cassert>
#include <cstdint>
#include <functional>
#include <string>

namespace Kuiper {
    namespace Cache {
        struct BaseTagsParams {
            public:
                int block_size;
                int entry_size;
                Kuiper::Cache::BaseIndexingPolicy * indexing_policy;
                bool sequential_access;
                uint64_t size;
                Cycles tag_latency;
                int warmup_percentage;
        };
    } // namespace Cache
} // namespace Kuiper

#endif // __PARAMS__BaseTags__
