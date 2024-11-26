#ifndef __PARAMS__BaseSetAssoc__
#define __PARAMS__BaseSetAssoc__

#include <cstddef>

#include "BaseReplacementPolicy.h"
#include "BaseTags.h"

namespace gem5 {
    namespace Cache {
        struct BaseSetAssocParams : 
                public BaseTagsParams {
            int assoc;
            Kuiper::Cache::replacement_policy::Base *replacement_policy;
        };
    } // namespace Cache
} // namespace Kuiper

#endif // __PARAMS__BaseSetAssoc__
