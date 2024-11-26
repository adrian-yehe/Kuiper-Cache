#ifndef __PARAMS__BaseCache__
#define __PARAMS__BaseCache__

#include <vector>
#include <cstddef>


namespace gem5 {
struct BaseCacheParams {
    std::vector< AddrRange > addr_ranges;
    unsigned assoc;
    enums::Clusivity clusivity;
    gem5::compression::Base * compressor;
    Cycles data_latency;
    unsigned demand_mshr_reserve;
    bool is_read_only;
    Counter max_miss_count;
    bool move_contractions;
    unsigned mshrs;
    gem5::prefetch::Base * prefetcher;
    bool replace_expansions;
    gem5::replacement_policy::Base * replacement_policy;
    Cycles response_latency;
    bool sequential_access;
    uint64_t size;
    gem5::System * system;
    Cycles tag_latency;
    gem5::BaseTags * tags;
    unsigned tgts_per_mshr;
    int warmup_percentage;
    gem5::WriteAllocator * write_allocator;
    unsigned write_buffers;
    bool writeback_clean;
    unsigned int port_cpu_side_connection_count;
    unsigned int port_mem_side_connection_count;
};

} // namespace gem5

#endif // __PARAMS__BaseCache__
