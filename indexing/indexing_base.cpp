#include <cassert>
#include <cstdlib>
#include "intmath.h"
#include "include/indexing_base.h"

//#include "base/logging.hh"
#include "replaceable/include/replaceable_entry.h"

namespace Kuiper {
    namespace Cache {
        BaseIndexingPolicy::BaseIndexingPolicy(const Params& p):  
            assoc(p.assoc),
            numSets(p.size / (p.entry_size * assoc)),
            setShift(floorLog2(p.entry_size)), setMask(numSets - 1), sets(numSets),
            tagShift(setShift + floorLog2(numSets)) {

            assert(isPowerOf2(numSets) && "# of sets must be non-zero and a power " \
                    "of 2");
            assert(assoc > 0 && "associativity must be greater than zero");

            // Make space for the entries
            for (uint32_t i = 0; i < numSets; ++i) {
                sets[i].resize(assoc);
            }
        }

        ReplaceableEntry*
            BaseIndexingPolicy::getEntry(const uint32_t set, const uint32_t way) const
        {
            return sets[set][way];
        }

        void BaseIndexingPolicy::setEntry(ReplaceableEntry* entry, const uint64_t index)
        {
            // Calculate set and way from entry index
            const std::lldiv_t div_result = std::div((long long)index, assoc);
            const uint32_t set = div_result.quot;
            const uint32_t way = div_result.rem;

            // Sanity check
            assert(set < numSets);

            // Assign a free pointer
            sets[set][way] = entry;

            // Inform the entry its position
            entry->setPosition(set, way);
        }

        Addr BaseIndexingPolicy::extractTag(const Addr addr) const
        {
            return (addr >> tagShift);
        }
    } // namespace Cache
} // namespace Kuiper
