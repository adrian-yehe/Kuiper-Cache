#include "systemc.h"
#include "spdlog/spdlog.h"
#include "cache/include/cache_packet.h"
#include "include/sim.h"
#include "include/top.h"

namespace Kuiper {
	namespace Cache {
		Top::Top(const std::string &_name, const NoncoherentCacheParams &_params, 
					const Params &_mem_params):
			sc_module(_name),
			l0("l0", _params), 
			GlobalMemory("Memory",
						_mem_params.id,
						_mem_params.cap,
						_mem_params.width) {
			SC_HAS_PROCESS(Top);
			SC_THREAD(Load0Thread);
			// SC_THREAD(Load1Thread);
			// SC_THREAD(StoreThread);

			l0.BindLoad0(load0, mResSignal[0]);
			l0.BindLoad1(load1, mResSignal[1]);
			l0.BindStore(store, mResSignal[2]);

			l0.mMemSidePort(GlobalMemory.mCpuSidePort);
		};

		auto Top::AllocateReqPacket(MemCmd _cmd, std::uint32_t _req_id) {
			std::uint32_t len = 128;
			std::uint32_t addr = 0x00;
			auto buf = new std::uint8_t[len] { 0 };
			auto pkt = this->Package(addr,  buf, len, 0, _cmd, _req_id);

			return pkt;
		}

		void Top::Load0Thread(void) {
			spdlog::info("Start {:s} {:s} process", 
							sc_module::name(), 
							__FUNCTION__);
			std::uint32_t req_id = 0;
			sc_core::sc_time delay(10, SC_SEC);

			while (true) {
				if(req_id++ < 2) {
					auto pkt = AllocateReqPacket(MemCmd::ReadReq);
					load0->write(pkt);
					spdlog::info("{:s}.load0 write packet, request id: {:d}",
								 sc_module::name(), req_id);
				}

				wait(delay);;
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

		PacketPtr Top::Package(Addr _addr, std::uint8_t *_buf,std::size_t _len, 
                                Request::Flags _flag, MemCmd _cmd, std::uint32_t _req_id) {
			// Create a new request-packet pair
			RequestPtr req = std::make_shared<Request>(
				_addr, _len, _flag, 0);

			PacketPtr pkt = new Packet(req, _cmd, _len, _req_id);
			pkt->dataStatic(_buf);

			return pkt;
        }
	}
}

void TestCache() {
	spdlog::info("Start cache simulation");
	Kuiper::Cache::SimObject sim;

	Kuiper::Cache::Top l0("Cache", *(sim.GetL0ParamsPtr()), 
								*(sim.GetMemoryParamsPtr()));
	sc_core::sc_start();
}
