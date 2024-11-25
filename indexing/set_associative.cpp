#include "set_associative.h"
#include "../replacement/replaceable_entry.h"

namespace Kuiper {
    namespace Cache {

 /*       SetAssociative::SetAssociative(const Params& p)
            : BaseIndexingPolicy(p)
        {
        }

        uint32_t
            SetAssociative::extractSet(const Addr addr) const
        {
            return (addr >> setShift) & setMask;
        }

        Addr
            SetAssociative::regenerateAddr(const Addr tag, const ReplaceableEntry* entry)
            const
        {
            return (tag << tagShift) | (entry->getSet() << setShift);
        }

        std::vector<ReplaceableEntry*>
            SetAssociative::getPossibleEntries(const Addr addr) const
        {
            return sets[extractSet(addr)];
        }*/
    } // namespace Cache
} // namespace Kuiper
