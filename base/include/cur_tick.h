#ifndef __SIM_CUR_TICK_HH__
#define __SIM_CUR_TICK_HH__

#include "types.h"

namespace Kuiper {
	namespace Cache {
		// This pointer is maintained by curEventQueue in src/sim/eventq.hh.
		Tick _curTickPtr;

		/// The universal simulation clock.
		inline Tick curTick() { return _curTickPtr; }
	} // namespace Cache
} // namespace Kuiper

#endif /* __SIM_CUR_TICK_HH__ */
