#ifndef  __GLOBAL_MEMORY_H__
#define __GLOBAL_MEMORY_H__

#include "tlm.h" 
#include "cache/include/cache_port.h"
#include "cache/include/cache_interface.h"

namespace Kuiper {
	namespace Cache {
        class Memory: public Target  {

            Memory(const Memory&) /* = delete*/;
            Memory& operator=(const Memory&) /* = delete */;

        public:
            Memory (const std::string  &_name,
                const std::uint32_t _ID                  // Target ID
                , std::uint64_t      _memory_size         // memory size (bytes)
                , std::uint32_t      _memory_width        // memory width (bytes)
            );
            void b_transport( tlm::tlm_generic_payload& gp, sc_core::sc_time& delay_time);
            std::uint8_t* get_mem_ptr(void);

        private:
            /// Check the address vs. range passed at construction
            tlm::tlm_response_status check_address(tlm::tlm_generic_payload& gp);

        private:

            const std::uint32_t     m_ID;                   ///< initiator ID
            const sc_core::sc_time m_read_delay;           ///< read delay
            const sc_core::sc_time m_write_delay;          ///< write delay
            const std::uint64_t    m_memory_size;          ///< memory size (bytes)
            const std::uint32_t     m_memory_width;         ///< memory width (bytes)

            unsigned char* m_memory;              ///< memory

            bool                   m_previous_warning;     ///< limits to one message
        };
	}
}

#endif // ! __GLOBAL_MEMORY_H__

