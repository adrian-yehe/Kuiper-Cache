#include "include/global_memory.h"                         // Header for this class
#include "spdlog/spdlog.h"

namespace Kuiper {
    namespace Cache {
        Memory::Memory (
            sc_core::sc_module_name _name,
            const std::uint32_t _ID                  // Target ID
            , std::uint64_t      _memory_size         // memory size (bytes)
            , std::uint32_t       _memory_width        // memory width (bytes)
        ): 
            sc_module(_name), 
            m_ID(_ID),
            m_memory_size(_memory_size),
            m_memory_width(_memory_width),
            m_previous_warning(false),
            mCpuSidePort("CpuSideSocket") {
            /// Allocate and initalize an array for the target's memory
            m_memory = new unsigned char[size_t(m_memory_size)];

            /// clear memory
            memset(m_memory, 0, size_t(m_memory_size));

            sc_assert(m_memory_width > 0);
            sc_assert(m_memory_size % m_memory_width == 0);

            if (m_memory_width > m_memory_size)
                assert(0);

            mCpuSidePort.register_b_transport(this, &Memory::b_transport);
        } // end Constructor

        void Memory::b_transport(tlm::tlm_generic_payload& gp, sc_core::sc_time& delay_time) {
            /// Access the required attributes from the payload
            sc_dt::uint64    address = gp.get_address();     // memory address
            tlm::tlm_command command = gp.get_command();     // memory command
            unsigned char* data = gp.get_data_ptr();    // data pointer
            unsigned  int     length = gp.get_data_length(); // data length



            tlm::tlm_response_status response_status = check_address(gp);

            if (gp.get_byte_enable_ptr()) {
                gp.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
            }
            else if (gp.get_streaming_width() != gp.get_data_length()) {
                gp.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
            }

            switch (command)  {
                default: {
                    if (m_previous_warning == false)
                        assert(0);
                    break;
                }

                case tlm::TLM_WRITE_COMMAND:
                {
                    if (response_status == tlm::TLM_OK_RESPONSE) {
                        spdlog::info("Recvive transaction from memory side port, write address:{:#x}", 
                            address);
                        for (unsigned int i = 0; i < length; i++) {
                            m_memory[address++] = data[i];     // move the data to memory
                        }
                        delay_time = delay_time + m_write_delay;
                    }
                    break;
                }

                case tlm::TLM_READ_COMMAND:
                {
                    if (response_status == tlm::TLM_OK_RESPONSE)  {
                        spdlog::info("Recvive transaction from memory side port, read address:{:#x}", 
                            address);
                        for (unsigned int i = 0; i < length; i++) {
                            data[i] = m_memory[address++];         // move the data to memory
                        }
                        delay_time = delay_time + m_read_delay;
                    }
                    break;
                }
            } // end switch

            gp.set_response_status(response_status);

            return;
        } // end memory_operation

        std::uint8_t* Memory::get_mem_ptr(void) { return m_memory; };

        tlm::tlm_response_status Memory::check_address(tlm::tlm_generic_payload& gp) {
            sc_dt::uint64    address = gp.get_address();     // memory address
            unsigned  int     length = gp.get_data_length(); // data length

            if (address >= m_memory_size)
                return tlm::TLM_ADDRESS_ERROR_RESPONSE; // operation response
            else {
                if ((address + length) >= m_memory_size)
                    return tlm::TLM_ADDRESS_ERROR_RESPONSE; // operation response

                return tlm::TLM_OK_RESPONSE;
            }
        }
    };
};

