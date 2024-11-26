#ifndef __PARAMS__BaseIndexingPolicy__
#define __PARAMS__BaseIndexingPolicy__

#include <cstddef>
#include <cstdint>

namespace Kuiper {
    namespace Cache {
        struct BaseIndexingPolicyParams {
            int assoc;
            int entry_size;
            uint64_t size;
        };
    } // namespace Cache
} // namespace Kuiper

#endif // __PARAMS__BaseIndexingPolicy__
