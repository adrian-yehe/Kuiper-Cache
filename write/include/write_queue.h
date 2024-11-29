#ifndef __MEM_CACHE_WRITE_QUEUE_HH__
#define __MEM_CACHE_WRITE_QUEUE_HH__

#include <string>

#include "types.h"
#include "write_queue_entry.h"
#include "../../cache/include/cache_queue.h"
#include "../../cache/include/cache_packet.h"

namespace Kuiper {
	namespace Cache {
    /**
     * A write queue for all eviction packets, i.e. writebacks and clean
     * evictions, as well as uncacheable writes.
     */
    class WriteQueue : public Queue<WriteQueueEntry> {
    public:
      /**
       * Create a write queue with a given number of entries.
       * @param num_entries The number of entries in this queue.
       * @param reserve The maximum number of entries needed to satisfy
       *        any access.
       */
      WriteQueue(const std::string &_label, int num_entries, int reserve,
                 const std::string &name);

      /**
       * Allocates a new WriteQueueEntry for the request and size. This
       * places the request as the first target in the WriteQueueEntry.
       *
       * @param blk_addr The address of the block.
       * @param blk_size The number of bytes to request.
       * @param pkt The original write.
       * @param when_ready When is the WriteQueueEntry be ready to act upon.
       * @param order The logical order of this WriteQueueEntry
       *
       * @return The a pointer to the WriteQueueEntry allocated.
       *
       * @pre There are free entries.
       */
      WriteQueueEntry *allocate(Addr blk_addr, unsigned blk_size,
                                PacketPtr pkt, Tick when_ready, Counter order);

      /**
       * Mark the given entry as in service. This removes the entry from
       * the readyList or deallocates the entry if it does not expect a
       * response (writeback/eviction rather than an uncacheable write).
       *
       * @param entry The entry to mark in service.
       */
      void markInService(WriteQueueEntry *entry);
    };
  } // namespace Cache
} // namespace Kuiper

#endif //__MEM_CACHE_WRITE_QUEUE_HH__
