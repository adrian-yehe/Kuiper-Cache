#ifndef __MEM_CACHE_PROBE_ARG_HH__
#define __MEM_CACHE_PROBE_ARG_HH__

#include "types.h"
#include "cache_packet.h"

namespace Kuiper {
    namespace Cache {
        /**
         * Provides generic cache lookup functions. A cache may provide
         * a CacheAccessor object to other components that need to perform
         * a lookup outside the normal cache control flow. Currently this
         * is used by prefetchers that perform lookups when notified by
         * cache events.
         */
        struct CacheAccessor {
            /** Determine if address is in cache */
            virtual bool inCache(Addr addr, bool is_secure) const = 0;

            /** Determine if address has been prefetched */
            virtual bool hasBeenPrefetched(Addr addr, bool is_secure) const = 0;

            /** Determine if address has been prefetched by the requestor */
            virtual bool hasBeenPrefetched(Addr addr, bool is_secure,
                                        RequestorID requestor) const = 0;

            /** Determine if address is in cache miss queue */
            virtual bool inMissQueue(Addr addr, bool is_secure) const = 0;

            /** Determine if cache is coalescing writes */
            virtual bool coalesce() const = 0;
        };

        /**
         * Information provided to probes on a cache event.
         * @sa ppHit, ppMiss, ppFill in gem5::BaseCache (src/mem/cache/base.hh)
         */
        class CacheAccessProbeArg {
        public:
            /** Packet that triggered the cache access*/
            PacketPtr pkt;
            /** Accessor for the cache */
            CacheAccessor &cache;

            CacheAccessProbeArg(PacketPtr _pkt, CacheAccessor &_cache)
                :pkt(_pkt), cache(_cache)
            {};
        };

        /**
         * A data contents update is composed of the updated block's address,
         * the old contents, and the new contents.
         * @sa ppDataUpdate in gem5::BaseCache (src/mem/cache/base.hh)
         */
        struct CacheDataUpdateProbeArg {
            /** The updated block's address. */
            Addr addr;
            /** Whether the block belongs to the secure address space. */
            bool isSecure;
            /** Block original requestor */
            const RequestorID requestorID;
            /** The stale data contents. If zero-sized this update is a fill. */
            std::vector<uint64_t> oldData;
            /** The new data contents. If zero-sized this is an invalidation. */
            std::vector<uint64_t> newData;
            /** Set if the update is from a prefetch or evicting a prefetched
            *  block that was never used. */
            bool hwPrefetched;
            /** Accessor for the cache */
            CacheAccessor &accessor;

            CacheDataUpdateProbeArg(Addr _addr, bool is_secure,
                                    RequestorID _requestorID,
                                    CacheAccessor &_accessor)
                : addr(_addr), isSecure(is_secure), requestorID(_requestorID),
                oldData(), newData(), accessor(_accessor)
            {};
        };
    } // namespace Cache
} // namespace Kuiper

#endif //__MEM_CACHE_PROBE_ARG_HH__
