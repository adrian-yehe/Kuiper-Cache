#include <cassert>
#include "include/write_queue.h"
#include "include/write_queue_entry.h"

namespace Kuiper {
	namespace Cache {
        WriteQueue::WriteQueue(const std::string &_label,
                               int num_entries, int reserve, const std::string &name)
            : Queue<WriteQueueEntry>(_label, num_entries, reserve,
                                     name + ".write_queue")
        {}

        WriteQueueEntry *
        WriteQueue::allocate(Addr blk_addr, unsigned blk_size, PacketPtr pkt,
                             Tick when_ready, Counter order) {
            assert(!freeList.empty());
            WriteQueueEntry *entry = freeList.front();
            assert(entry->getNumTargets() == 0);
            freeList.pop_front();

            entry->allocate(blk_addr, blk_size, pkt, when_ready, order);
            entry->allocIter = allocatedList.insert(allocatedList.end(), entry);
            entry->readyIter = addToReadyList(entry);

            allocated += 1;
            return entry;
        }

        void
        WriteQueue::markInService(WriteQueueEntry *entry) {
            // for a normal eviction, such as a writeback or a clean evict,
            // there is no more to do as we are done from the perspective of
            // this cache, and for uncacheable write we do not need the entry
            // as part of the response handling
            entry->popTarget();
            deallocate(entry);
        }
    } // namespace Cache
} // namespace Kuiper
