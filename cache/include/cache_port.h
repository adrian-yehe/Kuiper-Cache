#ifndef  _CACHE_PORT_H__
#define  _CACHE_PORT_H__

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
#include "spdlog/spdlog.h"
#include "cache_packet.h"

namespace Kuiper {
	namespace Cache {
        using ReqInPort = sc_core::sc_port<sc_fifo_in_if<PacketPtr>>;
        using ReqOutPort = sc_core::sc_port<sc_fifo_out_if<PacketPtr>>;

        using RequestPort = sc_core::sc_port<sc_fifo_in_if<PacketPtr>>;
        using ResponsetPort = sc_core::sc_port<sc_fifo_out_if<PacketPtr>>;

        using ResSignal = sc_core::sc_signal<PacketPtr>;

        /**
        * The CPU-side port extends the base cache response port with access
        * functions for functional, atomic and timing requests.
        */
        template <typename CacheType>
        class CpuSidePort : public sc_core::sc_module {
        public:
            CpuSidePort(const std::string &_name, CacheType* _cache) :
                sc_module(_name.c_str()),
                mBaseCache(_cache) {
                SC_HAS_PROCESS(CpuSidePort);
                SC_THREAD(ReqMonitor);
            };

            CpuSidePort(sc_core::sc_module_name _name) :
                sc_module(_name) {
                SC_HAS_PROCESS(CpuSidePort);
                SC_THREAD(ReqMonitor);
            };

        public:
            void ReqMonitor(void) {
                std::uint8_t buf[512] = { 0 };
                while (true) {
                    auto pkt = RecvPacket();
                    spdlog::info("{:s}.recive packet ",
                        sc_module::name());
                    mBaseCache->recvTimingReq(pkt); 
                }
            }

        public:
            auto RecvPacket(void) { return mReqPort->read(); };
            void SendPacket(PacketPtr& _pkt) { mResPort->write(_pkt); };

        public:
            RequestPort mReqPort;
            sc_core::sc_port<sc_core::sc_signal_out_if<PacketPtr>, 0> mResPort;

        private:
            CacheType* mBaseCache;
        };

        /**
        * The memory-side port extends the base cache request port with
        * access functions for functional, atomic and timing snoops.
        */
        template <typename CacheType>
        class MemSidePort : public sc_core::sc_module {
        public:
            MemSidePort(const std::string &_name, CacheType* _cache):
                sc_module(_name.c_str()),
                 mBaseCache(_cache) {
                //SC_HAS_PROCESS(MemSidePort);
                //SC_THREAD(ResMonitor);
            };
            MemSidePort(sc_core::sc_module_name _name) :
                sc_module(_name) {
                //SC_HAS_PROCESS(MemSidePort);
                //SC_THREAD(ResMonitor);
            };

        public:
            void ResMonitor(void) {
                while (true) {
                    auto pkt = RecvPacket();
                    spdlog::info("Recive packet from {:s}",
                        sc_module::name());
                }
            }

        public:
            void SendPacket(PacketPtr &_pkt) {  mReqPort->write(_pkt); };
            bool sendTimingReq(PacketPtr _pkt) { return mReqPort->nb_write(_pkt); };
            PacketPtr RecvPacket(void) { return mResPort->read(); };

        public:
            ResponsetPort mReqPort;
            sc_core::sc_port<sc_core::sc_signal_in_if<PacketPtr>, 0> mResPort;

        private:
            CacheType* mBaseCache;
        };
    } /* namespace Cache */
} /* namespace Kuiper */

#endif // ! _CACHE_PORT_H__
