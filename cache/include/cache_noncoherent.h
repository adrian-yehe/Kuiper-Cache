#ifndef __MEM_CACHE_NONCOHERENT_CACHE_HH__
#define __MEM_CACHE_NONCOHERENT_CACHE_HH__

#include "types.h"
#include "cache_base.h"
#include "cache_packet.h"

namespace Kuiper {
    namespace Cache {
        class CacheBlk;
        class MSHR;
        struct NoncoherentCacheParams;

        /**
         * A non-coherent cache
         */
        class NoncoherentCache : public BaseCache {
        public:
            bool access(PacketPtr pkt, CacheBlk *&blk, Cycles &lat,
                        PacketList &writebacks) override;

            void handleTimingReqMiss(PacketPtr pkt, CacheBlk *blk,
                                     Tick forward_time,
                                     Tick request_time) override;
            void handleTimingRes(PacketPtr pkt) override;

            void recvTimingReq(PacketPtr pkt) override;

            virtual bool sendTimingReq(PacketPtr pkt) { return sendTimingReq(pkt); };

            // void doWritebacks(PacketList &writebacks,
            //                   Tick forward_time) override;

            // void doWritebacksAtomic(PacketList &writebacks) override;

            void serviceMSHRTargets(MSHR *mshr, const PacketPtr pkt,
                                    CacheBlk *blk) override;

            void recvTimingResp(PacketPtr pkt) override;

            void functionalAccess(PacketPtr pkt, bool from_cpu_side) override;

            void satisfyRequest(PacketPtr pkt, CacheBlk *blk,
                                bool deferred_response = false,
                                bool pending_downgrade = false) override;

            /*
             * Creates a new packet with the request to be send to the memory
             * below. The noncoherent cache is below the point of coherence
             * and therefore all fills bring in writable, therefore the
             * needs_writeble parameter is ignored.
             */
            PacketPtr createMissPacket(PacketPtr cpu_pkt, CacheBlk *blk,
                                       bool needs_writable,
                                       bool is_whole_line_write) const override;

            [[nodiscard]] PacketPtr evictBlock(CacheBlk *blk) override;

        public:
            NoncoherentCache(const NoncoherentCacheParams &p);
        };
    } // // namespace Cache
} // namespace Kuiper

#endif // __MEM_CACHE_NONCOHERENTCACHE_HH__
