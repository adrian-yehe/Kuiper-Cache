#include "spdlog/spdlog.h"
#include "include/cache_packet_queue.h"

namespace Kuiper {
    namespace Cache {
        PacketQueue::PacketQueue(const std::string &_name,
                                 bool force_order,
                                 bool disable_sanity_check) : sc_module(_name.c_str()),
                                                              _disableSanityCheck(disable_sanity_check),
                                                              forceOrder(force_order),
                                                              waitingOnRetry(false)
        {
            SC_HAS_PROCESS(PacketQueue);
            SC_THREAD(pktQueueProcess);
        };

        PacketQueue::~PacketQueue() {}

        void PacketQueue::pktQueueProcess(void) {
            while (true) {
                wait(sendEvent);
                processSendEvent();
            }
        }

        void
        PacketQueue::retry() {
            spdlog::info("{:s} Queue received retry", 
                        sc_module::name());
            assert(waitingOnRetry);
            waitingOnRetry = false;
            sendDeferredPacket();
        }

        bool
        PacketQueue::checkConflict(const PacketPtr pkt, const int blk_size) const
        {
            // caller is responsible for ensuring that all packets have the
            // same alignment
            for (const auto &p : transmitList)
            {
                if (p.pkt->matchBlockAddr(pkt, blk_size))
                    return true;
            }
            return false;
        }

        void
        PacketQueue::schedSendTiming(PacketPtr pkt, Tick when) {
            // DPRINTF(PacketQueue, "%s for %s address %x size %d when %lu ord: %i\n",
            //         __func__, pkt->cmdString(), pkt->getAddr(), pkt->getSize(), when,
            //         forceOrder);

            // add a very basic sanity check on the port to ensure the
            // invisible buffer is not growing beyond reasonable limits
            if (!_disableSanityCheck && transmitList.size() > 128)
            {
                assert(0 && "Packet queue has grown beyond 128 packets");
            }

            // we should either have an outstanding retry, or a send event
            // scheduled, but there is an unfortunate corner case where the
            // x86 page-table walker and timing CPU send out a new request as
            // part of the receiving of a response (called by
            // PacketQueue::sendDeferredPacket), in which we end up calling
            // ourselves again before we had a chance to update waitingOnRetry
            // assert(waitingOnRetry || sendEvent.scheduled());

            // this belongs in the middle somewhere, so search from the end to
            // order by tick; however, if forceOrder is set, also make sure
            // not to re-order in front of some existing packet with the same
            // address
            auto it = transmitList.end();
            while (it != transmitList.begin())
            {
                --it;
                if ((forceOrder && it->pkt->matchAddr(pkt)) || it->tick <= when)
                {
                    // emplace inserts the element before the position pointed to by
                    // the iterator, so advance it one step
                    transmitList.emplace(++it, when, pkt);
                    return;
                }
            }
            // either the packet list is empty or this has to be inserted
            // before every other packet
            transmitList.emplace_front(when, pkt);
            schedSendEvent(when);
        }

        void
        PacketQueue::schedSendEvent(Tick when)
        {
            // if we are waiting on a retry just hold off
            if (waitingOnRetry) 
                spdlog::info("Not scheduling send as waiting for retry");
              
            sendEvent.notify();
        }

        void
        PacketQueue::sendDeferredPacket()
        {
            // sanity checks
            assert(!waitingOnRetry);
            assert(deferredPacketReady());

            DeferredPacket dp = transmitList.front();

            // take the packet of the list before sending it, as sending of
            // the packet in some cases causes a new packet to be enqueued
            // (most notaly when responding to the timing CPU, leading to a
            // new request hitting in the L1 icache, leading to a new
            // response)
            transmitList.pop_front();

            // use the appropriate implementation of sendTiming based on the
            // type of queue
            waitingOnRetry = !sendTiming(dp.pkt);

            // if we succeeded and are not waiting for a retry, schedule the
            // next send
            if (!waitingOnRetry)
            {
                schedSendEvent(deferredPacketReadyTime());
            }
            else
            {
                // put the packet back at the front of the list
                transmitList.emplace_front(dp);
            }
        }

        void
        PacketQueue::processSendEvent()
        {
            assert(!waitingOnRetry);
            sendDeferredPacket();
        }

        ReqPacketQueue::ReqPacketQueue(const std::string &_name) : PacketQueue(_name) {};

        bool
        ReqPacketQueue::sendTiming(PacketPtr pkt)
        {
            return true;
            // return memSidePort.sendTimingReq(pkt);
        }

        RespPacketQueue::RespPacketQueue(const std::string &_name): PacketQueue(_name) {};

        bool
        RespPacketQueue::sendTiming(PacketPtr pkt)
        {
            return true;
            // return cpuSidePort.sendTimingResp(pkt);
        }
    } /* namespace Cache */
} /* namespace Kuiper */



