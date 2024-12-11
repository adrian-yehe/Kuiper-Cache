#ifndef  __CACHE_TOP_H__
#define __CACHE_TOP_H__

// #include "models/SimpleBusAT.h" 
#include "memory/include/global_memory.h"
#include "params/Memrory.h"
#include "l0.h"
#include "l1.h"

namespace Kuiper {
	namespace Cache {
		class Top: public sc_core::sc_module {
		public:
			typedef MemoryParams Params;
			Top(const std::string &_name, const NoncoherentCacheParams &_params, 
					const Params &_mem_params);

		private:
			PacketPtr Package(Addr _addr, std::uint8_t *_buf,std::size_t _len, 
                                Request::Flags _flag, MemCmd _cmd, std::uint32_t _req_id = 0);
			auto AllocateReqPacket(MemCmd _cmd, std::uint32_t _req_id = 0);

		public:
			void Load0Thread();
			void Load1Thread();
			void StoreThread();

		public:
		 	L0 l0;
			Memory GlobalMemory;

		public:	
			ReqOutPort load0;
			ReqOutPort load1;
			ReqOutPort store;

		public:
			std::array<ResSignal, 3> mResSignal;
		

		};
	} /* namespace Cache */
} /* namespace Kuiper */


void TestCache(void);



#endif // ! __CACHE_TOP_H__



