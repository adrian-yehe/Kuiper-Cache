#ifndef __PARAMS__NoncoherentCache__
#define __PARAMS__NoncoherentCache__

#include "BaseCache.h"
#include "BasePrefetcher.h"
#include "BaseSetAssoc.h"
#include "RandomReplacement.h"
#include "TaggedPrefetcher.h"
#include "SetAssociative.h"

namespace Kuiper {
    namespace Cache {
        struct NoncoherentCacheParams: 
            public BaseCacheParams, 
            public BaseSetAssocParams, 
            public TaggedPrefetcherParams,
            public RandomRPParams,
            public SetAssociativeParams {

        };
    } // namespace Cache
} // namespace Kuiper

#endif // __PARAMS__NoncoherentCache__
