#include <cassert>
#include "types.h"
#include "include/cache_blk.h"
#include "mshr/include/mshr.h"
#include "include/cache_noncoherent.h"
#include "params/CacheNoncoherent.h"

namespace Kuiper {
    namespace Cache {
        NoncoherentCache::NoncoherentCache(const NoncoherentCacheParams &p)
            : BaseCache(p, p.cacheline_size) {
            assert(p.BaseCacheParams::tags);
            assert(p.BaseCacheParams::replacement_policy);
        }

        void
        NoncoherentCache::satisfyRequest(PacketPtr pkt, CacheBlk *blk, bool, bool) {
            // As this a non-coherent cache located below the point of
            // coherency, we do not expect requests that are typically used to
            // keep caches coherent (e.g., InvalidateReq or UpdateReq).
            assert(pkt->isRead() || pkt->isWrite());
            BaseCache::satisfyRequest(pkt, blk);
        }

        bool
        NoncoherentCache::access(PacketPtr pkt, CacheBlk *&blk, Cycles &lat,
                                 PacketList &writebacks) {
            bool success = BaseCache::access(pkt, blk, lat, writebacks);

            if (pkt->isWriteback() || pkt->cmd == MemCmd::WriteClean) {
                assert(blk && blk->isValid());
                // Writeback and WriteClean can allocate and fill even if the
                // referenced block was not present or it was invalid. If that
                // is the case, make sure that the new block is marked as
                // writable
                blk->setCoherenceBits(CacheBlk::WritableBit);
            }

            return success;
        }

        void NoncoherentCache::handleTimingRes(PacketPtr pkt) {
            MemCmd cmd;

            if(pkt->isRead())
                cmd = MemCmd::ReadExResp;
            else
                cmd = MemCmd::WriteResp;

            PacketPtr p = new Packet(pkt->req,
                                    cmd, pkt->getSize(), pkt->id);
            p->dataStatic(pkt->getPtr<std::uint8_t>());

            p->senderState = pkt->senderState;

            recvTimingResp(p);
        }

        // void
        // NoncoherentCache::doWritebacks(PacketList &writebacks, Tick forward_time)
        // {
        //     while (!writebacks.empty())
        //     {
        //         PacketPtr wb_pkt = writebacks.front();
        //         allocateWriteBuffer(wb_pkt, forward_time);
        //         writebacks.pop_front();
        //     }
        // }

        // void
        // NoncoherentCache::doWritebacksAtomic(PacketList &writebacks)
        // {
        //     while (!writebacks.empty())
        //     {
        //         PacketPtr wb_pkt = writebacks.front();
        //         memSidePort.sendAtomic(wb_pkt);
        //         writebacks.pop_front();
        //         delete wb_pkt;
        //     }
        // }

        void
        NoncoherentCache::handleTimingReqMiss(PacketPtr pkt, CacheBlk *blk,
                                              Tick forward_time, Tick request_time)
        {
            // miss
            Addr blk_addr = pkt->getBlockAddr(blkSize);
            MSHR *mshr = mshrQueue.findMatch(blk_addr, pkt->isSecure(), false);

            // We can always write to a non coherent cache if the block is
            // present and therefore if we have reached this point then the
            // block should not be in the cache.
            assert(mshr || !blk || !blk->isValid());

            BaseCache::handleTimingReqMiss(pkt, mshr, blk, forward_time, request_time);
        }

        void
        NoncoherentCache::recvTimingReq(PacketPtr pkt) {
            assert(!pkt->cacheResponding() && 
                    "Should not see packets where cache is responding");
            assert((pkt->isRead() || pkt->isWrite()) && 
                    "Should only see read and writes at non-coherent cache\n");

            BaseCache::recvTimingReq(pkt);
        }

        PacketPtr
        NoncoherentCache::createMissPacket(PacketPtr cpu_pkt, CacheBlk *blk,
                                           bool needs_writable,
                                           bool is_whole_line_write) const
        {
            // We also fill for writebacks from the coherent caches above us,
            // and they do not need responses
            assert(cpu_pkt->needsResponse());

            // A miss can happen only due to missing block
            assert(!blk || !blk->isValid());

            PacketPtr pkt = new Packet(cpu_pkt->req, MemCmd::ReadReq, blkSize);

            // the packet should be block aligned
            assert(pkt->getAddr() == pkt->getBlockAddr(blkSize));

            pkt->allocate();

            spdlog::info("{:s} created {:s} from {:s}", __func__, pkt->print(),
                    cpu_pkt->print());
            return pkt;
        }

        void
        NoncoherentCache::functionalAccess(PacketPtr pkt, bool from_cpu_side)
        {
            // gem5::panic_if(!from_cpu_side, "Non-coherent cache received functional snoop"
            //                          " request\n");

            BaseCache::functionalAccess(pkt, from_cpu_side);
        }

        void
        NoncoherentCache::serviceMSHRTargets(MSHR *mshr, const PacketPtr pkt,
                                             CacheBlk *blk) {
            // First offset for critical word first calculations
            const int initial_offset = mshr->getTarget()->pkt->getOffset(blkSize);

            bool from_core = false;
            bool from_pref = false;

            MSHR::TargetList targets = mshr->extractServiceableTargets(pkt);
            for (auto &target : targets)
            {
                Packet *tgt_pkt = target.pkt;

                switch (target.source)
                {
                case MSHR::Target::FromCPU:
                    // handle deferred requests comming from a cache or core
                    // above

                    from_core = true;

                    Tick completion_time;
                    // Here we charge on completion_time the delay of the xbar if the
                    // packet comes from it, charged on headerDelay.
                    completion_time = pkt->headerDelay;

                    satisfyRequest(tgt_pkt, blk);

                    // How many bytes past the first request is this one
                    int transfer_offset;
                    transfer_offset = tgt_pkt->getOffset(blkSize) - initial_offset;
                    if (transfer_offset < 0)
                    {
                        transfer_offset += blkSize;
                    }
                    // If not critical word (offset) return payloadDelay.
                    // responseLatency is the latency of the return path
                    // from lower level caches/memory to an upper level cache or
                    // the core.
                    // completion_time += clockEdge(responseLatency) +
                    //                    (transfer_offset ? pkt->payloadDelay : 0);

                    assert(tgt_pkt->req->requestorId() < maxRequestors);
                    // stats.cmdStats(tgt_pkt).missLatency[tgt_pkt->req->requestorId()] +=
                    //     completion_time - target.recvTime;

                    tgt_pkt->makeTimingResponse();
                    if (pkt->isError())
                        tgt_pkt->copyError(pkt);

                    // Reset the bus additional time as it is now accounted for
                    tgt_pkt->headerDelay = tgt_pkt->payloadDelay = 0;
                    // cpuSidePort.schedTimingResp(tgt_pkt, completion_time);
                    break;

                case MSHR::Target::FromPrefetcher:
                    // handle deferred requests comming from a prefetcher
                    // attached to this cache
                    assert(tgt_pkt->cmd == MemCmd::HardPFReq);

                    from_pref = true;

                    // We have filled the block and the prefetcher does not
                    // require responses.
                    delete tgt_pkt;
                    break;

                default:
                    // we should never see FromSnoop Targets as this is a
                    // non-coherent cache
                    // panic("Illegal target->source enum %d\n", target.source);
                    assert(0 && "Illegal target->source enum");
                }
            }

            if (blk && !from_core && from_pref){
                blk->setPrefetched();
            }

            // Reponses are filling and bring in writable blocks, therefore
            // there should be no deferred targets and all the non-deferred
            // targets are now serviced.
            assert(mshr->getNumTargets() == 0);
        }

        void
        NoncoherentCache::recvTimingResp(PacketPtr pkt) {
            assert(pkt->isResponse());
            // At the moment the only supported downstream requests we issue
            // are ReadReq and therefore here we should only see the
            // corresponding responses
            assert(pkt->isRead());
            assert(pkt->cmd != MemCmd::UpgradeResp);
            assert(!pkt->isInvalidate());
            // This cache is non-coherent and any memories below are
            // non-coherent too (non-coherent caches or the main memory),
            // therefore the fetched block can be marked as writable.
            assert(!pkt->hasSharers());

            BaseCache::recvTimingResp(pkt);
        }

        PacketPtr
        NoncoherentCache::evictBlock(CacheBlk *blk)
        {
            // A dirty block is always written back.

            // A clean block can we written back, if we turned on writebacks
            // for clean blocks. This could be useful if there is a cache
            // below and we want to make sure the block is cached but if the
            // memory below is the main memory WritebackCleans are
            // unnecessary.

            // If we clean writebacks are not enabled, we do not take any
            // further action for evictions of clean blocks (i.e., CleanEvicts
            // are unnecessary).
            PacketPtr pkt = (blk->isSet(CacheBlk::DirtyBit) || writebackClean) ? writebackBlk(blk) : nullptr;

            invalidateBlock(blk);

            return pkt;
        }
    } // namespace Cache
} // namespace Kuiper
