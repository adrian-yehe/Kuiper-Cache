#ifndef __L1_H__
#define _L1_H__

#include<cstdint>
#include<iostream>

#include "cache_port.h"
#include "cache.h"

namespace Kuiper {
	namespace Cache {
		class L1: public Cache {
		public:
			L1(const std::uint32_t& _cap, const std::uint32_t &_assoc, const std::uint32_t &_line_size);
			L1(sc_core::sc_module_name _name):
				Cache(_name),
				mReq(_name),
				mMemPort(_name) {};
		
		public:
			CpuSidePort<Cache> mReq;
			MemSidePort<Cache> mMemPort;
		};
	}
}
#endif /* _L1_H__ */
