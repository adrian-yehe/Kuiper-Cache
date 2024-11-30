#include "cache/include/cache_packet.h"
#include "include/top.h"
#include "systemc.h"
#include "spdlog/spdlog.h"

namespace Kuiper {
	namespace Cache {

		Top::Top(sc_core::sc_module_name _name, const NoncoherentCacheParams &_params) :
			sc_module(_name),
			l0("l0", _params)  {
			SC_HAS_PROCESS(Top);
			SC_THREAD(Load0Thread);
			// SC_THREAD(Load1Thread);
			// SC_THREAD(StoreThread);

			l0.BindLoad0(load0, mResSignal[0]);
			l0.BindLoad1(load1, mResSignal[1]);
			l0.BindStore(store, mResSignal[2]);
		};

		auto Top::AllocateReqPacket(MemCmd _cmd) {
			std::uint32_t len = 128;
			std::uint32_t addr = 0x00;
			auto buf = new std::uint8_t[len] { 0 };
			auto pkt = Package(addr,  buf, 0, len, _cmd);

			return pkt;
		}

		void Top::Load0Thread(void) {
			spdlog::info("Start {:s} {:s} process", 
							sc_module::name(), 
							__FUNCTION__);
			std::uint32_t req_id = 0;
			auto pkt = AllocateReqPacket(MemCmd::ReadReq);

			while (true) {
				load0->write(pkt);
				spdlog::info("{:s}.load0 write packet, request id: {:d}", 
					sc_module::name(), req_id++);
			}
		}

		void Top::Load1Thread(void) {
			std::uint32_t req_id = 0;
			auto pkt = AllocateReqPacket(MemCmd::ReadReq);

			while (true) {
				// load1->write(pkt);
				// spdlog::info("{:s}.load1 write packet: {:d}",
				// 	sc_module::name(), index++);
			}
		}

		void Top::StoreThread(void) {
			std::uint32_t req_id = 0;
			auto pkt = AllocateReqPacket(MemCmd::WriteReq);

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
