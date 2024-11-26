#ifndef __PARAMS__BasePrefetcher__
#define __PARAMS__BasePrefetcher__

#include <cstdint>
#include <cstddef>

namespace Kuiper {
    namespace Cache {
        struct BasePrefetcherParams {
            int block_size;
            bool on_data;
            bool on_inst;
            bool on_miss;
            bool on_read;
            bool on_write;
            uint64_t page_bytes;
            bool prefetch_on_access;
            bool prefetch_on_pf_hit;
            bool use_virtual_addresses;
        };
    }  // namespace Cache
} // namespace Kuiper

#endif // __PARAMS__BasePrefetcher__
