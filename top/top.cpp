#include "include/top.h"
#include "systemc.h"
#include "spdlog/spdlog.h"

namespace Kuiper {
	namespace Cache {

		Top::Top(sc_core::sc_module_name _name, const NoncoherentCacheParams &_params) :
			sc_module(_name),
			L0("l0", _params)  {
			SC_HAS_PROCESS(Top);
			SC_THREAD(Load0Thread);
			// SC_THREAD(Load1Thread);
			// SC_THREAD(StoreThread);

			L0.BindLoad0(load0, mResSignal[0]);
			// L0.BindLoad1(load1, mResSignal[1]);
			// L0.BindStore(store, mResSignal[2]);
		};

		void Top::Load0Thread(void) {
			spdlog::info("Start {:s} {:s} process", 
							sc_module::name(), 
							__FUNCTION__);

			auto pkt = new Packet();
			std::uint32_t req_id = 0;
			while (true) {
				load0->write(pkt);
				spdlog::info("{:s}.load0 write packet, request id: {:d}", 
					sc_module::name(), req_id++);
			}
		}

		void Top::Load1Thread(void) {
			auto pkt = new Packet();
			std::uint32_t index = 0;
			while (true) {
				// load1->write(pkt);
				// spdlog::info("{:s}.load1 write packet: {:d}",
				// 	sc_module::name(), index++);
			}
		}

		void Top::StoreThread(void) {
			auto pkt = new Packet();
			std::uint32_t index = 0;
			while (true) {
				// store->write(pkt);
				// spdlog::info("{:s}.store write packet: {:d}",
				// 	sc_module::name(), index++);
			}
		}
	}
}

void TestCache() {
	spdlog::info("Start cache simulation");
	// // Kuiper::Cache::Top cache("Cache");
	// sc_core::sc_start(100, SC_SEC);
}
