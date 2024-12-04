#ifndef __RANDOM_REPLACEMENT_PARAMS_H__
#define __RANDOM_REPLACEMENT_PARAMS_H__
#include <cstdint>
#include "replaceable/include/replaceable_random.h"

namespace Kuiper {
    namespace Cache {
        namespace replacement_policy { 
            class Random; 
        };

        struct RandomRPParams {
            replacement_policy::Random *random;
        };
    }
}

#endif /* __RANDOM_REPLACEMENT_PARAMS_H__ */
