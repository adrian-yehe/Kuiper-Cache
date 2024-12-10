#ifndef __L0_H__
#define __L0_H__

#include<cstdint>
#include<iostream>

#include "tlm.h"
#include <tlm_utils/simple_initiator_socket.h>
#include "cache/include/cache_port.h"
#include "cache/include/cache_noncoherent.h"
#include "params/BaseCache.h"

namespace Kuiper {
	namespace Cache {
		class L0 : public NoncoherentCache {
		public:
			typedef struct NoncoherentCacheParams Params;
			L0(sc_core::sc_module_name _name, const Params &_params);

		public:
			void BindLoad0(ReqOutPort& _port, ResSignal&_res_signal); 
			void BindLoad1(ReqOutPort& _port, ResSignal& _res_signal);
			void BindStore(ReqOutPort& _port, ResSignal& _res_signal);

		private:
			CpuSidePort<L0> mLoad0;
			CpuSidePort<L0> mLoad1;
			CpuSidePort<L0> mStore;

			// MemSidePort<NoncoherentCache> mMemPort;
		public:
			tlm_utils::simple_initiator_socket<L0> mMemSidePort;

		public:
			auto Read(const std::uint64_t _addr, const std::size_t &_size, std::uint8_t *_buf);
			auto Write(const std::uint64_t _addr, const std::size_t &_size, std::uint8_t *_buf);

		private:
			sc_core::sc_fifo<PacketPtr> mLoad0Fifo;
			sc_core::sc_fifo<PacketPtr> mLoad1Fifo;
			sc_core::sc_fifo<PacketPtr> mStoreFifo;
		};
	} /* namespace Cache */
}  /* namespace Kuiper */

#endif /* __L0_H__ */
