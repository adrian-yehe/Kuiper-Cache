#include "cache_top.h"
#include "systemc.h"
#include "spdlog/spdlog.h"

namespace Kuiper {
	namespace Cache {

		CacheTop::CacheTop(sc_core::sc_module_name _name) :
			sc_module(_name),
			L0("l0")  {
			SC_HAS_PROCESS(CacheTop);
			SC_THREAD(Load0Thread);
			SC_THREAD(Load1Thread);
			SC_THREAD(StoreThread);

			L0.BindLoad0(load0, mResSignal[0]);
			L0.BindLoad1(load1, mResSignal[1]);
			L0.BindStore(store, mResSignal[2]);
		};

		void CacheTop::Load0Thread(void) {
			auto pkt = new Packet();
			std::uint32_t index = 0;
			while (true) {
				load0->write(pkt);
				spdlog::info("{:s}.load0 write packet: {:d}", 
					sc_module::name(), index++);
			}
		}

		void CacheTop::Load1Thread(void) {
			auto pkt = new Packet();
			std::uint32_t index = 0;
			while (true) {
				load1->write(pkt);
				spdlog::info("{:s}.load1 write packet: {:d}",
					sc_module::name(), index++);
			}
		}

		void CacheTop::StoreThread(void) {
			auto pkt = new Packet();
			std::uint32_t index = 0;
			while (true) {
				store->write(pkt);
				spdlog::info("{:s}.store write packet: {:d}",
					sc_module::name(), index++);
			}
		}
	}
}

void TestCache() {
	Kuiper::Cache::CacheTop cache("cache_top");
	sc_core::sc_start(10, SC_SEC);
}




