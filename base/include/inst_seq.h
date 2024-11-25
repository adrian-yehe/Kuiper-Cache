#ifndef __STD_TYPES_HH__
#define __STD_TYPES_HH__

#include <cstdint>

namespace Kuiper {
	namespace Cache {

		// inst sequence type, used to order instructions in the ready list,
		// if this rolls over the ready list order temporarily will get messed
		// up, but execution will continue and complete correctly
		typedef std::uint64_t InstSeqNum;

		// inst tag type, used to tag an operation instance in the IQ
		typedef std::uint32_t InstTag;
	}  // namespace Cache
} // namespace Kuiper

#endif // __STD_TYPES_HH__

