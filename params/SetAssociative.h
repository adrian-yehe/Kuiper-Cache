#ifndef __PARAMS__SetAssociative__
#define __PARAMS__SetAssociative__

#include "params/BaseIndexingPolicy.h"

namespace Kuiper {
    namespace Cache {
        class SetAssociative;
        struct SetAssociativeParams
            : public BaseIndexingPolicyParams
        {
            Kuiper::Cache::SetAssociative *set_assoc;
        };
    } // namespace Cache
} // namespace Kuiper

#endif // __PARAMS__SetAssociative__
