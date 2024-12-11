#ifndef _CACHE_PACKET_QUEUE_H__
#define _CACHE_PACKET_QUEUE_H__

#include <string>
#include <list>
#include "types.h"
#include "cache_packet.h"
#include "systemc.h"

namespace Kuiper {
    namespace Cache {
        /**
         * A packet queue is a class that holds deferred packets and later
         * sends them using the associated CPU-side port or memory-side port.
         */
        class PacketQueue : public sc_core::sc_module {
        private:
            /** A deferred packet, buffered to transmit later. */
            class DeferredPacket
            {
            public:
                Tick tick;     ///< The tick when the packet is ready to transmit
                PacketPtr pkt; ///< Pointer to the packet to transmit
                DeferredPacket(Tick t, PacketPtr p)
                    : tick(t), pkt(p)
                {
                }
            };

            typedef std::list<DeferredPacket> DeferredPacketList;

            /** A list of outgoing packets. */
            DeferredPacketList transmitList;

            /** Used to schedule sending of deferred packets. */
            void processSendEvent();

            /** Event used to call processSendEvent. */
            sc_core::sc_event  sendEvent;

            /*
             * Optionally disable the sanity check
             * on the size of the transmitList. The
             * sanity check will be enabled by default.
             */
            bool _disableSanityCheck;

            /**
             * if true, inserted packets have to be unconditionally scheduled
             * after the last packet in the queue that references the same
             * address
             */
            bool forceOrder;

        protected:
            /** Label to use for print request packets label stack. */
            const std::string label;

            /** Remember whether we're awaiting a retry. */
            bool waitingOnRetry;

            /** Check whether we have a packet ready to go on the transmit list. */
            bool deferredPacketReady() const
            {
                return !transmitList.empty();
            }

            /**
             * Attempt to send a packet. Note that a subclass of the
             * PacketQueue can override this method and thus change the
             * behaviour (as done by the cache for the request queue). The
             * default implementation sends the head of the transmit list. The
             * caller must guarantee that the list is non-empty and that the
             * head packet is scheduled for curTick() (or earlier).
             */
            virtual void sendDeferredPacket();

            /**
             * Send a packet using the appropriate method for the specific
             * subclass (request, response or snoop response).
             */
            virtual bool sendTiming(PacketPtr pkt) = 0;

        public:
            /**
             * Create a packet queue, linked to an event manager, and a label
             * that will be used for functional print request packets.
             *
             * @param _em Event manager used for scheduling this queue
             * @param _label Label to push on the label stack for print request packets
             * @param force_order Force insertion order for packets with same address
             * @param disable_sanity_check Flag used to disable the sanity check
             *        on the size of the transmitList. The check is enabled by default.
             */
            PacketQueue(const std::string &_name,
                        bool force_order = false,
                        bool disable_sanity_check = false);

            /**
             * Virtual desctructor since the class may be used as a base class.
             */
            virtual ~PacketQueue();

            /**
             * SystemC Thread process , process the request packet
             */
            void pktQueueProcess(void);

            /**
             * Provide a name to simplify debugging.
             *
             * @return A complete name, appended to module and port
             */
            virtual const std::string name() const = 0;

            /**
             * Get the size of the queue.
             */
            size_t size() const { return transmitList.size(); }

            /**
             * Get the next packet ready time.
             */
            Tick deferredPacketReadyTime() const
            {
                return transmitList.empty() ? MaxTick : transmitList.front().tick;
            }

            /**
             * Check if a packet corresponding to the same address exists in the
             * queue.
             *
             * @param pkt The packet to compare against.
             * @param blk_size Block size in bytes.
             * @return Whether a corresponding packet is found.
             */
            bool checkConflict(const PacketPtr pkt, const int blk_size) const;

            /** Check the list of buffered packets against the supplied
             * functional request. */
            bool trySatisfyFunctional(PacketPtr pkt);

            /**
             * Schedule a send event if we are not already waiting for a
             * retry. If the requested time is before an already scheduled
             * send event, the event will be rescheduled. If MaxTick is
             * passed, no event is scheduled. Instead, if we are idle and
             * asked to drain then check and signal drained.
             *
             * @param when time to schedule an event
             */
            void schedSendEvent(Tick when);

            /**
             * Add a packet to the transmit list, and schedule a send event.
             *
             * @param pkt Packet to send
             * @param when Absolute time (in ticks) to send packet
             */
            void schedSendTiming(PacketPtr pkt, Tick when);

            /**
             * Retry sending a packet from the queue. Note that this is not
             * necessarily the same packet if something has been added with an
             * earlier time stamp.
             */
            void retry();
        };

        class ReqPacketQueue : public PacketQueue {
        public:
            /**
             * Create a request packet queue, linked to an event manager, a
             * memory-side port, and a label that will be used for functional print
             * request packets.
             *
             * @param _em Event manager used for scheduling this queue
             * @param _mem_side_port Mem_side port used to send the packets
             * @param _label Label to push on the label stack for print request packets
             */
            ReqPacketQueue(const std::string &_name);

            virtual ~ReqPacketQueue() {}

            const std::string name() const override { return sc_module::name(); };

            bool sendTiming(PacketPtr pkt);
        };

        class RespPacketQueue : public PacketQueue
        {
        public:
            /**
             * Create a response packet queue, linked to an event manager, a
             * CPU-side port, and a label that will be used for functional print
             * request packets.
             *
             * @param _em Event manager used for scheduling this queue
             * @param _cpu_side_port Cpu_side port used to send the packets
             * @param force_order Force insertion order for packets with same address
             * @param _label Label to push on the label stack for print request packets
             */
            RespPacketQueue(const std::string &_name);

            virtual ~RespPacketQueue() {}

            bool sendTiming(PacketPtr pkt);
        };
    } /* namespace Cache */
} /* namespace Kuiper */















#endif /* _CACHE_PACKET_QUEUE_H__ */
