#include <cassert>
#include <string>
#include "include/write_queue_entry.h"
#include "types.h"
#include "../cache/include/cache_base.h"
#include "../cache/include/cache_request.h"

namespace Kuiper {
	namespace Cache {
        inline void
        WriteQueueEntry::TargetList::add(PacketPtr pkt, Tick readyTime,
                                         Counter order) {
            emplace_back(pkt, readyTime, order);
        }

        bool
        WriteQueueEntry::TargetList::trySatisfyFunctional(PacketPtr pkt)
        {
            for (auto &t : *this)
            {
                // if (pkt->trySatisfyFunctional(t.pkt))
                // {
                //     return true;
                // }
            }

            return false;
        }

        void
        WriteQueueEntry::TargetList::print(std::ostream &os, int verbosity,
                                           const std::string &prefix) const
        {
            for (auto &t : *this)
            {
                // ccprintf(os, "%sFromCPU: ", prefix);
                // t.pkt->print(os, verbosity, "");
            }
        }

        void
        WriteQueueEntry::allocate(Addr blk_addr, unsigned blk_size, PacketPtr target,
                                  Tick when_ready, Counter _order)
        {
            blkAddr = blk_addr;
            blkSize = blk_size;
            isSecure = target->isSecure();
            readyTime = when_ready;
            order = _order;
            assert(target);
            _isUncacheable = target->req->isUncacheable();
            inService = false;

            // we should never have more than a single target for cacheable
            // writes (writebacks and clean evictions)
            assert((!_isUncacheable && !targets.empty()));
            // assert((!_isUncacheable && !targets.empty()) &&
            //            "Write queue entry %#llx should never have more than one "
            //            "cacheable target",
            //        blkAddr);
            assert(!((target->isWrite() && _isUncacheable) || 
                    (target->isEviction() && !_isUncacheable) ||
                    target->cmd == MemCmd::WriteClean));
            // panic_if(!((target->isWrite() && _isUncacheable) ||
            //            (target->isEviction() && !_isUncacheable) ||
            //            target->cmd == MemCmd::WriteClean),
            //          "Write queue entry %#llx should be an uncacheable write or "
            //          "a cacheable eviction or a writeclean");

            targets.add(target, when_ready, _order);

            // All targets must refer to the same block
            assert(target->matchBlockAddr(targets.front().pkt, blkSize));
        }

        void
        WriteQueueEntry::deallocate()
        {
            assert(targets.empty());
            inService = false;
        }

        bool
        WriteQueueEntry::trySatisfyFunctional(PacketPtr pkt)
        {
            // For printing, we treat the WriteQueueEntry as a whole as single
            // entity. For other requests, we iterate over the individual
            // targets since that's where the actual data lies.
            if (pkt->isPrint())
            {
                // pkt->trySatisfyFunctional(this, blkAddr, isSecure, blkSize, nullptr);
                // return false;
            }
            else
            {
                return targets.trySatisfyFunctional(pkt);
            }
        }

        bool
        WriteQueueEntry::sendPacket(BaseCache &cache)
        {
            return cache.sendWriteQueuePacket(this);
        }

        bool
        WriteQueueEntry::matchBlockAddr(const Addr addr, const bool is_secure) const
        {
            assert(hasTargets());
            return (blkAddr == addr) && (isSecure == is_secure);
        }

        bool
        WriteQueueEntry::matchBlockAddr(const PacketPtr pkt) const
        {
            assert(hasTargets());
            return pkt->matchBlockAddr(blkAddr, isSecure, blkSize);
        }

        bool
        WriteQueueEntry::conflictAddr(const QueueEntry *entry) const
        {
            assert(hasTargets());
            return entry->matchBlockAddr(blkAddr, isSecure);
        }

        void
        WriteQueueEntry::print(std::ostream &os, int verbosity,
                               const std::string &prefix) const
        {
            // gem5::ccprintf(os, "%s[%#llx:%#llx](%s) %s %s %s state: %s %s %s %s %s\n",
            //          prefix, blkAddr, blkAddr + blkSize - 1,
            //          isSecure ? "s" : "ns",
            //          _isUncacheable ? "Unc" : "",
            //          inService ? "InSvc" : "");

            // gem5::ccprintf(os, "%s  Targets:\n", prefix);
            // targets.print(os, verbosity, prefix + "    ");
        }

        std::string
        WriteQueueEntry::print() const
        {
            std::ostringstream str;
            // print(str);
            return str.str();
        }
    } // namespace Cache
} // namespace Kuiper
