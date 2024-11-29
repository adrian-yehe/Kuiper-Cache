#ifndef __WRITE_ACCOCATOR_PARAMS_H__
#define __WRITE_ACCOCATOR_PARAMS_H__
#include <cstdint>

namespace Kuiper {
    namespace Cache {
        struct WriteAllocatorParams {
           
            int block_size;
            unsigned coalesce_limit;
            unsigned delay_threshold;
            unsigned no_allocate_limit;
        };
    }
}

#endif /* __WRITE_ACCOCATOR_PARAMS_H__ */
