#include <cassert>
#include "include/mshr_queue.h"
#include "include/mshr.h"

namespace Kuiper {
    namespace Cache {

        MSHRQueue::MSHRQueue(const std::string &_label,
                             int num_entries, int reserve,
                             int demand_reserve, std::string cache_name = "")
            : Queue<MSHR>(_label, num_entries, reserve, cache_name + ".mshr_queue"),
              demandReserve(demand_reserve)
        {}

        MSHR *
        MSHRQueue::allocate(Addr blk_addr, unsigned blk_size, PacketPtr pkt,
                            Tick when_ready, Counter order, bool alloc_on_fill) {
            assert(!freeList.empty());
            MSHR *mshr = freeList.front();
            assert(mshr->getNumTargets() == 0);
            freeList.pop_front();

            // DPRINTF(MSHR, "Allocating new MSHR. Number in use will be %lu/%lu\n",
            //         allocatedList.size() + 1, numEntries);

            mshr->allocate(blk_addr, blk_size, pkt, when_ready, order, alloc_on_fill);
            mshr->allocIter = allocatedList.insert(allocatedList.end(), mshr);
            mshr->readyIter = addToReadyList(mshr);

            allocated += 1;
            return mshr;
        }

        void
        MSHRQueue::deallocate(MSHR *mshr) {

            // DPRINTF(MSHR, "Deallocating all targets: %s", mshr->print());
            Queue<MSHR>::deallocate(mshr);
            // DPRINTF(MSHR, "MSHR deallocated. Number in use: %lu/%lu\n",
            //         allocatedList.size(), numEntries);
        }

        void
        MSHRQueue::moveToFront(MSHR *mshr)
        {
            if (!mshr->inService)
            {
                assert(mshr == *(mshr->readyIter));
                readyList.erase(mshr->readyIter);
                mshr->readyIter = readyList.insert(readyList.begin(), mshr);
            }
        }

        void
        MSHRQueue::delay(MSHR *mshr, Tick delay_ticks)
        {
            mshr->delay(delay_ticks);
            auto it = std::find_if(mshr->readyIter, readyList.end(),
                                   [mshr](const MSHR *_mshr)
                                   {
                                       return mshr->readyTime >= _mshr->readyTime;
                                   });
            readyList.splice(it, readyList, mshr->readyIter);
        }

        void
        MSHRQueue::markInService(MSHR *mshr, bool pending_modified_resp)
        {
            mshr->markInService(pending_modified_resp);
            readyList.erase(mshr->readyIter);
            _numInService += 1;
        }

        void
        MSHRQueue::markPending(MSHR *mshr)
        {
            assert(mshr->inService);
            mshr->inService = false;
            --_numInService;
            /**
             * @ todo might want to add rerequests to front of pending list for
             * performance.
             */
            mshr->readyIter = addToReadyList(mshr);
        }

        bool
        MSHRQueue::forceDeallocateTarget(MSHR *mshr)
        {
            bool was_full = isFull();
            assert(mshr->hasTargets());
            // Pop the prefetch off of the target list
            mshr->popTarget();
            // Delete mshr if no remaining targets
            if (!mshr->hasTargets() && !mshr->promoteDeferredTargets())
            {
                deallocate(mshr);
            }

            // Notify if MSHR queue no longer full
            return was_full && !isFull();
        }
    } // namespace Cache
} // namespace Kuiper
