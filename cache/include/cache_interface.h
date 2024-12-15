#ifndef __CACHE_INTERFACE_HH__
#define __CACHE_INTERFACE_HH__

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
#include <tlm_utils/simple_initiator_socket.h>
#include "tlm_utils/simple_target_socket.h"

namespace Kuiper {
    namespace Cache {
    class Initiator {
        public:
            Initiator(const std::string &_name) {};

        public:
			tlm_utils::simple_initiator_socket<Initiator> mMemSidePort;

        private:
            bool Read(const std::uint64_t _addr, const std::size_t &_size, std::uint8_t *_buf) {
                tlm::tlm_generic_payload trans; // Define a generic transaction
                sc_time delay = SC_ZERO_TIME;

                /* Example: read operation to address _addr */
                trans.set_command(tlm::TLM_READ_COMMAND);
                trans.set_address(_addr);
                trans.set_data_length(_size);
                trans.set_data_ptr(_buf); // Data to write
                trans.set_streaming_width(_size);
                trans.set_byte_enable_ptr(reinterpret_cast<std::uint8_t *>(_addr));

                /* Send transaction using b_transport */
                mMemSidePort->b_transport(trans, delay);
                if (tlm::TLM_OK_RESPONSE == trans.get_response_status()) {
                    spdlog::info("{:s} [{:#x}:{:#x}] success", 
                                    __func__,
                                    _addr, _addr + _size - 1);
                    return true;
                }
                else
                    assert(0 && "from downstream read response invalid");
                
                return false;
            }

            bool Write(const std::uint64_t _addr, const std::size_t &_size, std::uint8_t *_buf)
            {
                tlm::tlm_generic_payload trans; // Define a generic transaction
                sc_time delay = SC_ZERO_TIME;

                // Example: Write operation to address 0x100
                trans.set_command(tlm::TLM_WRITE_COMMAND);
                trans.set_address(_addr);
                trans.set_data_length(_size);
                trans.set_data_ptr(_buf); // Data to write

                // Send transaction using b_transport
                mMemSidePort->b_transport(trans, delay);
                if (tlm::TLM_OK_RESPONSE == trans.get_response_status()) {
                    spdlog::info("{:s} [{:#x}:{:#x}]success", 
                                    __func__,
                                    _addr, _addr + _size);
                    return true;
                }
                else
                    assert(0 && "from downstream write response invalid");
            } 
                
        public:
            virtual bool sendTimingReq(PacketPtr pkt)
            {
                if (pkt->isWrite())
                    return Write(pkt->getAddr(), pkt->getSize(), pkt->getPtr<std::uint8_t>());
                else if (pkt->isRead())
                    return Read(pkt->getAddr(), pkt->getSize(), pkt->getPtr<std::uint8_t>());
                else
                    assert(0 && "packet type invalid");
            }
        };

        class Target : public sc_core::sc_module {
        public:
            Target(const std::string &_name, 
                    const std::function<void(tlm::tlm_generic_payload&, sc_core::sc_time&)> &_callback) : 
                    sc_module(_name.c_str()),
                    mCallback(_callback) {
                mCpuSidePort.register_b_transport(this, &Target::b_transport);
            };

        public:
            tlm_utils::simple_target_socket<Target> mCpuSidePort;

        void b_transport( tlm::tlm_generic_payload& gp, sc_core::sc_time& delay_time) {
             spdlog::info("{:s} {:s} access target", sc_module::name(), __FUNCTION__);
            this->mCallback(gp, delay_time);
        }

        private:
            std::function<void(tlm::tlm_generic_payload&, sc_core::sc_time&)> mCallback;
        };
    } /* namespace Cache */
} /* namespace Kuiper */




#endif /* __CACHE_INTERFACE_HH__ */
