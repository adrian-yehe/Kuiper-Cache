#ifndef __L0_H__
#define __L0_H__

#include<cstdint>
#include<iostream>

#include "tlm.h"
#include "cache/include/cache_port.h"
#include "cache/include/cache_noncoherent.h"
#include "params/BaseCache.h"

namespace Kuiper {
	namespace Cache {
		class L0 : public NoncoherentCache, public sc_core::sc_module {
		public:
			typedef struct NoncoherentCacheParams Params;
			L0(sc_core::sc_module_name _name, const Params &_params):
				sc_module(_name),
				NoncoherentCache(_params),
				mLoad0("load0"),
				mLoad1("load1"),
				mStore("store"),
			//	mMemPort("l0"),
				mLoad0Fifo("load0_fifo", 16),
				mLoad1Fifo("load1_fifo", 16),
				mStoreFifo("store_fifo", 10) {
				mLoad0.mReqPort(mLoad0Fifo);
				mLoad1.mReqPort(mLoad1Fifo);
				mStore.mReqPort(mStoreFifo);
			};

		public:
			void BindLoad0(ReqOutPort& _port, ResSignal&_res_signal) {
				_port(mLoad0Fifo); 
				mLoad0.mResPort(_res_signal);
			};
			void BindLoad1(ReqOutPort& _port, ResSignal& _res_signal) {
				_port(mLoad1Fifo);
				mLoad1.mResPort(_res_signal);
			};
			void BindStore(ReqOutPort& _port, ResSignal& _res_signal) {
				_port(mStoreFifo);
				mStore.mResPort(_res_signal);
			};

		private:
			CpuSidePort<NoncoherentCache> mLoad0;
			CpuSidePort<NoncoherentCache> mLoad1;
			CpuSidePort<NoncoherentCache> mStore;

	//		MemSidePort<Cache> mMemPort;
	//		tlm::tlm_initiator_socket< > mInitiatorSocket;

		private:
			sc_core::sc_fifo<PacketPtr> mLoad0Fifo;
			sc_core::sc_fifo<PacketPtr> mLoad1Fifo;
			sc_core::sc_fifo<PacketPtr> mStoreFifo;
		};
	} /* namespace Cache */
}  /* namespace Kuiper */

#endif /* __L0_H__ */
