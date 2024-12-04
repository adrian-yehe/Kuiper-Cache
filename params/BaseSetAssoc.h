#ifndef __PARAMS__BaseSetAssoc__
#define __PARAMS__BaseSetAssoc__

#include "BaseReplacementPolicy.h"
#include "BaseTags.h"

namespace Kuiper {
    namespace Cache {
        namespace replacement_policy { 
            class Base; 
        };

        struct BaseSetAssocParams : public BaseTagsParams {
            int assoc;
            replacement_policy::Base *replacement_policy;
        };
    } // namespace Cache
} // namespace Kuiper

#endif // __PARAMS__BaseSetAssoc__
