#ifndef  __CACHE_TOP_H__
#define __CACHE_TOP_H__

// #include "models/SimpleBusAT.h" 
// #include "global_memory.h"
#include "l0.h"
#include "l1.h"

namespace Kuiper {
	namespace Cache {
		class Top: public sc_core::sc_module {
		public:
			Top(sc_core::sc_module_name, const NoncoherentCacheParams &_params);

		public:
			void Load0Thread();
			void Load1Thread();
			void StoreThread();

		public:
			
		 	L0 L0;
		//	Memory GlobalMemory;
			
			ReqOutPort load0;
			ReqOutPort load1;
			ReqOutPort store;

			std::array<ResSignal, 3> mResSignal;
		};
	} /* namespace Cache */
} /* namespace Kuiper */


void TestCache(void);



#endif // ! __CACHE_TOP_H__



