#include <cstdint>
#include <map>
#include <list>
#include <bitset>
#include <array>
#include <utility>
#include <memory>
#include <iostream>
#include <functional>
#include "systemc.h"

#include "cache_packet.h"
#include "cache_blk.h"

namespace Kuiper {
    namespace Cache {
        class BaseCache: public sc_core::sc_module {
        public:
            BaseCache(sc_core::sc_module_name _name):
                sc_module(_name) {};

        public:
            virtual bool access(PacketPtr pkt, CacheBlk*& blk, Cycles& lat,
                PacketList& writebacks);
            virtual void handleTimingReqHit(PacketPtr pkt, CacheBlk* blk,
                Tick request_time);
            virtual void handleTimingReqMiss(PacketPtr pkt, CacheBlk* blk,
                Tick forward_time, Tick request_time);
            //void handleTimingReqMiss(PacketPtr pkt, MSHR* mshr, CacheBlk* blk,
            //    Tick forward_time, Tick request_time);
            virtual void recvTimingReq(PacketPtr pkt);
            virtual void recvTimingResp(PacketPtr pkt);
            void updateBlockData(CacheBlk* blk, const PacketPtr cpkt,
                bool has_old_data);
            virtual PacketPtr createMissPacket(PacketPtr cpu_pkt, CacheBlk* blk,
                bool needs_writable,
                bool is_whole_line_write) const = 0;
            CacheBlk* handleFill(PacketPtr pkt, CacheBlk* blk,
                PacketList& writebacks, bool allocate);
            CacheBlk* allocateBlock(const PacketPtr pkt, PacketList& writebacks);

        public:

        ///** Miss status registers */
        //MSHRQueue mshrQueue;

        ///** Write/writeback buffer */
        //WriteQueue writeBuffer;

        ///** Tag and data Storage */
        //BaseTags* tags;

        ///** Prefetcher */
        //prefetch::Base* prefetcher;

        };
    }; /* namespace Cache */

} /* namespace Kuiper */


