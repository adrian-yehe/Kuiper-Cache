#ifndef __MEM_CACHE_QUEUE_ENTRY_HH__
#define __MEM_CACHE_QUEUE_ENTRY_HH__

#include "types.h"
#include "named.h"
#include "cache_packet.h"
#include "cur_tick.h"

namespace Kuiper {
    namespace Cache {

      class BaseCache;

      /**
       * A queue entry base class, to be used by both the MSHRs and
       * write-queue entries.
       */
      class QueueEntry : public Packet::SenderState, public Named {

        /**
         * Consider the Queue a friend to avoid making everything public
         */
        template <class Entry>
        friend class Queue;

      protected:
        /** Tick when ready to issue */
        Tick readyTime;

        /** True if the entry is uncacheable */
        bool _isUncacheable;

      public:
        /**
         * A queue entry is holding packets that will be serviced as soon as
         * resources are available. Since multiple references to the same
         * address can arrive while a packet is not serviced, each packet is
         * stored in a target containing its availability, order and other info,
         * and the queue entry stores these similar targets in a list.
         */
        class Target
        {
        public:
          const Tick recvTime;  //!< Time when request was received (for stats)
          const Tick readyTime; //!< Time when request is ready to be serviced
          const Counter order;  //!< Global order (for memory consistency mgmt)
          PacketPtr pkt;        //!< Pending request packet.

          /**
           * Default constructor. Assigns the current tick as the arrival time
           * of the packet.
           *
           * @param _pkt The pending request packet.
           * @param ready_time The tick at which the packet will be serviceable.
           * @param _order Global order.
           */
          Target(PacketPtr _pkt, Tick ready_time, Counter _order)
              : recvTime(curTick()), readyTime(ready_time), order(_order),
                pkt(_pkt)
          {
          }
        };

        /** True if the entry has been sent downstream. */
        bool inService;

        /** Order number assigned to disambiguate writes and misses. */
        Counter order;

        /** Block aligned address. */
        Addr blkAddr;

        /** Block size of the cache. */
        unsigned blkSize;

        /** True if the entry targets the secure memory space. */
        bool isSecure;

        QueueEntry(const std::string &name)
            : Named(name),
              readyTime(0), _isUncacheable(false),
              inService(false), order(0), blkAddr(0), blkSize(0), isSecure(false)
        {
        }

        bool isUncacheable() const { return _isUncacheable; }

        /**
         * Check if entry corresponds to the one being looked for.
         *
         * @param addr Address to match against.
         * @param is_secure Whether the target should be in secure space or not.
         * @return True if entry matches given information.
         */
        virtual bool matchBlockAddr(const Addr addr, const bool is_secure)
            const = 0;

        /**
         * Check if entry contains a packet that corresponds to the one being
         * looked for.
         *
         * @param pkt The packet to search for.
         * @return True if any of its targets' packets matches the given one.
         */
        virtual bool matchBlockAddr(const PacketPtr pkt) const = 0;

        /**
         * Check if given entry's packets conflict with this' entries packets.
         *
         * @param entry Other entry to compare against.
         * @return True if entry matches given information.
         */
        virtual bool conflictAddr(const QueueEntry *entry) const = 0;

        /**
         * Send this queue entry as a downstream packet, with the exact
         * behaviour depending on the specific entry type.
         */
        virtual bool sendPacket(BaseCache &cache) = 0;

        /**
         * Returns a pointer to the first target.
         *
         * @return A pointer to the first target.
         */
        virtual Target *getTarget() = 0;
      };
    }// namespace Cache
} // namespace Kuiper

#endif // __MEM_CACHE_QUEUE_ENTRY_HH__
