#include "include/sim.h"
#include "indexing/include/set_associative.h"
#include "prefetch/include/prefetch_tagged.h"
#include "tags/include/tag_set_assoc.h"

namespace Kuiper {
    namespace Cache {
        class SetAssociative;
        void SimObject::InitL0BaseCache() {
            simL0Params.BaseCacheParams::name = "l0";
            simL0Params.BaseCacheParams::size = 256 * 1024;
            simL0Params.BaseCacheParams::assoc = 4;
            simL0Params.BaseCacheParams::cacheline_size = 512;

            simL0Params.BaseCacheParams::mshrs = 12;
            simL0Params.BaseCacheParams::tgts_per_mshr = 1;

            simL0Params.BaseCacheParams::is_read_only = false;
           
            simL0Params.BaseCacheParams::write_buffers = 12;
            simL0Params.BaseCacheParams::writeback_clean = false;
            simL0Params.BaseCacheParams::port_cpu_side_connection_count = 3;
            simL0Params.BaseCacheParams::port_mem_side_connection_count = 1;

            simL0Params.BaseCacheParams::demand_mshr_reserve = 1;
            simL0Params.BaseCacheParams::move_contractions = false;
            simL0Params.BaseCacheParams::replace_expansions = false;
        }

        void SimObject::InitL0Replacement() {
            simL0Params.RandomRPParams::random = new replacement_policy::Random;
        }

        void SimObject::InitL0Tags() { 
            simL0Params.BaseTagsParams::block_size = simL0Params.cacheline_size;
            simL0Params.BaseTagsParams::entry_size = simL0Params.cacheline_size;
            simL0Params.BaseTagsParams::sequential_access = true;
            simL0Params.BaseTagsParams::size = simL0Params.BaseCacheParams::size;
            // simL0Params.BaseTagsParams::tag_latency = Cycle(0);
            simL0Params.BaseTagsParams::warmup_percentage = 1;
        }

        void SimObject::InitL0SetAssoc() {
            simL0Params.BaseSetAssocParams::assoc = simL0Params.BaseCacheParams::assoc;
            simL0Params.BaseSetAssocParams::replacement_policy = 
                        simL0Params.RandomRPParams::random;

            simL0Params.SetAssociativeParams::assoc = simL0Params.BaseCacheParams::assoc;
            simL0Params.SetAssociativeParams::entry_size = 
                        simL0Params.BaseTagsParams::entry_size;
            simL0Params.SetAssociativeParams::size = simL0Params.BaseCacheParams::size;


            simL0Params.SetAssociativeParams::set_assoc = new SetAssociative(simL0Params);
            simL0Params.BaseTagsParams::indexing_policy = 
                        simL0Params.SetAssociativeParams::set_assoc;

            simL0Params.BaseTagsParams::tags = new BaseSetAssoc(simL0Params);
            simL0Params.BaseCacheParams::tags =  simL0Params.BaseTagsParams::tags;

            simL0Params.BaseCacheParams::replacement_policy = 
                        simL0Params.RandomRPParams::random;
        }

        void SimObject::InitL0Tagged() {
            /* Base prefetcher parasm */
            simL0Params.BasePrefetcherParams::block_size = 
                        simL0Params.BaseCacheParams::cacheline_size;
            simL0Params.BasePrefetcherParams::on_data = true;
            simL0Params.BasePrefetcherParams::on_inst = false;
            simL0Params.BasePrefetcherParams::on_miss = true;
            simL0Params.BasePrefetcherParams::on_read = true;
            simL0Params.BasePrefetcherParams::on_write = true;
            simL0Params.BasePrefetcherParams::page_bytes = 4096;
            simL0Params.BasePrefetcherParams::prefetch_on_access = true;
            simL0Params.BasePrefetcherParams::prefetch_on_pf_hit = false;
            simL0Params.BasePrefetcherParams::use_virtual_addresses = false;

            /* Perfetch queue params */
            simL0Params.QueuedPrefetcherParams::cache_snoop = false;
            simL0Params.QueuedPrefetcherParams::latency = 0;
            simL0Params.QueuedPrefetcherParams::max_prefetch_requests_with_pending_translation = 1;
            simL0Params.QueuedPrefetcherParams::queue_filter = false;
            simL0Params.QueuedPrefetcherParams::queue_size = 10;
            simL0Params.QueuedPrefetcherParams::queue_squash = false;
            simL0Params.QueuedPrefetcherParams::tag_prefetch = false;
            simL0Params.QueuedPrefetcherParams::throttle_control_percentage = 10;

            /* Tagged prefetch params */
            simL0Params.TaggedPrefetcherParams::degree = 1;
            simL0Params.TaggedPrefetcherParams::tagged = 
                                     new prefetch::Tagged(simL0Params);
        }

        void SimObject::InitSimObject() {
            InitL0BaseCache();
            InitL0Replacement();
            InitL0Tags();
            InitL0SetAssoc();
            InitL0Tagged();
        }
    }
}

