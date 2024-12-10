#include "include/l0.h"

namespace Kuiper {
    namespace Cache {
        L0::L0(sc_core::sc_module_name _name, const Params &_params) : NoncoherentCache(_params),
                                                                       mLoad0("load0Port", this),
                                                                       mLoad1("load1Port", this),
                                                                       mStore("storePort", this),
                                                                       mMemSidePort("socket"),
                                                                       // mMemSPort("l0"),
                                                                       mLoad0Fifo("load0_fifo", 16),
                                                                       mLoad1Fifo("load1_fifo", 16),
                                                                       mStoreFifo("store_fifo", 10)
        {
            mLoad0.mReqPort(mLoad0Fifo);
            mLoad1.mReqPort(mLoad1Fifo);
            mStore.mReqPort(mStoreFifo);
        };

        void L0::BindLoad0(ReqOutPort &_port, ResSignal &_res_signal)
        {
            _port(mLoad0Fifo);
            mLoad0.mResPort(_res_signal);
        };
        void L0::BindLoad1(ReqOutPort &_port, ResSignal &_res_signal)
        {
            _port(mLoad1Fifo);
            mLoad1.mResPort(_res_signal);
        };
        void L0::BindStore(ReqOutPort &_port, ResSignal &_res_signal)
        {
            _port(mStoreFifo);
            mStore.mResPort(_res_signal);
        };

        auto L0::Read(const std::uint64_t _addr, const std::size_t &_size, std::uint8_t *_buf)
        {
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
            if (tlm::TLM_OK_RESPONSE == trans.get_response_status())
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        auto L0::Write(const std::uint64_t _addr, const std::size_t &_size, std::uint8_t *_buf)
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
            if (tlm::TLM_OK_RESPONSE == trans.get_response_status())
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    } /* namespace Cache */
} /* namesapce Kuiper */
