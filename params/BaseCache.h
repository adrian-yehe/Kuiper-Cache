#ifndef __PARAMS__BaseCache__
#define __PARAMS__BaseCache__
#include <cstdint>
#include <vector>
#include <cstddef>

#include "cache/include/cache_base.h"
#include "cache/include/cache_port.h"
#include "tags/include/tag_base.h"
#include "prefetch/include/prefetch_base.h"
#include "replaceable/include/replaceable_base.h"

namespace Kuiper {
    namespace Cache {
        namespace prefetch { class Base;};
        class BaseCache;
        struct BaseCacheParams {
            // std::vector<AddrRange> addr_ranges;
            std::string name;
            unsigned assoc;
            // enums::Clusivity clusivity;
            // gem5::compression::Base *compressor;
            Cycles data_latency;
            unsigned demand_mshr_reserve;
            bool is_read_only;
            Counter max_miss_count;
            bool move_contractions;
            unsigned mshrs;
            prefetch::Base *prefetcher;
            bool replace_expansions;
            replacement_policy::Base *replacement_policy;
            Cycles response_latency;
            bool sequential_access;
            uint64_t size;
            int32_t cacheline_size;
            // gem5::System *system;
            Cycles tag_latency;
            Kuiper::Cache::BaseTags *tags;
            unsigned tgts_per_mshr;
            int warmup_percentage;
            // gem5::WriteAllocator *write_allocator;
            unsigned write_buffers;
            bool writeback_clean;
            unsigned int port_cpu_side_connection_count;
            unsigned int port_mem_side_connection_count;
            std::vector<CpuSidePort<BaseCache> *> cpu_side_port;
            std::vector<MemSidePort<BaseCache> *> mem_side_port;
        };
        // using BaseCacheParams = struct BaseCacheParams;
    } // namespace Cache
} // namespace Kuiper

#endif // __PARAMS__BaseCache__
