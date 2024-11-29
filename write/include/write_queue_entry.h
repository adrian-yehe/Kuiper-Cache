#ifndef __MEM_CACHE_WRITE_QUEUE_ENTRY_HH__
#define __MEM_CACHE_WRITE_QUEUE_ENTRY_HH__

#include <cassert>
#include <iosfwd>
#include <list>
#include <string>

#include "types.h"
#include "cache/include/cache_queue_entry.h"
#include "cache/include/cache_packet.h"

namespace Kuiper {
	namespace Cache {

    class BaseCache;

    /**
     * Write queue entry
     */
    class WriteQueueEntry : public QueueEntry {

      /**
       * Consider the queues friends to avoid making everything public.
       */
      template <typename Entry>
      friend class Queue;
      friend class WriteQueue;

    public:
      class TargetList : public std::list<Target> {

      public:
        TargetList() {}
        void add(PacketPtr pkt, Tick readyTime, Counter order);
        bool trySatisfyFunctional(PacketPtr pkt);
        void print(std::ostream &os, int verbosity,
                   const std::string &prefix) const;
      };

      /** A list of write queue entriess. */
      typedef std::list<WriteQueueEntry *> List;
      /** WriteQueueEntry list iterator. */
      typedef List::iterator Iterator;

      bool sendPacket(BaseCache &cache) override;

    private:
      /**
       * Pointer to this entry on the ready list.
       * @sa MissQueue, WriteQueue::readyList
       */
      Iterator readyIter;

      /**
       * Pointer to this entry on the allocated list.
       * @sa MissQueue, WriteQueue::allocatedList
       */
      Iterator allocIter;

      /** List of all requests that match the address */
      TargetList targets;

    public:
      /** A simple constructor. */
      WriteQueueEntry(const std::string &name)
          : QueueEntry(name)
      {
      }

      /**
       * Allocate a miss to this entry.
       * @param blk_addr The address of the block.
       * @param blk_size The number of bytes to request.
       * @param pkt The original write.
       * @param when_ready When should the write be sent out.
       * @param _order The logical order of this write.
       */
      void allocate(Addr blk_addr, unsigned blk_size, PacketPtr pkt,
                    Tick when_ready, Counter _order);

      /**
       * Mark this entry as free.
       */
      void deallocate();

      /**
       * Returns the current number of allocated targets.
       * @return The current number of allocated targets.
       */
      int getNumTargets() const
      {
        return targets.size();
      }

      /**
       * Returns true if there are targets left.
       * @return true if there are targets
       */
      bool hasTargets() const { return !targets.empty(); }

      /**
       * Returns a reference to the first target.
       * @return A pointer to the first target.
       */
      Target *getTarget() override
      {
        assert(hasTargets());
        return &targets.front();
      }

      /**
       * Pop first target.
       */
      void popTarget()
      {
        targets.pop_front();
      }

      bool trySatisfyFunctional(PacketPtr pkt);

      /**
       * Prints the contents of this MSHR for debugging.
       */
      void print(std::ostream &os,
                 int verbosity = 0,
                 const std::string &prefix = "") const ;
      /**
       * A no-args wrapper of print(std::ostream...)  meant to be
       * invoked from DPRINTFs avoiding string overheads in fast mode
       *
       * @return string with mshr fields
       */
      std::string print() const;

      bool matchBlockAddr(const Addr addr, const bool is_secure) const override;
      bool matchBlockAddr(const PacketPtr pkt) const override;
      bool conflictAddr(const QueueEntry *entry) const override;
    };
  } // namespace Cache
} // namespace Kuiper

#endif // __MEM_CACHE_WRITE_QUEUE_ENTRY_HH__
