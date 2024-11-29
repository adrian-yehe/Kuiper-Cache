#ifndef __MEM_CACHE_INDEXING_POLICIES_SET_ASSOCIATIVE_HH__
#define __MEM_CACHE_INDEXING_POLICIES_SET_ASSOCIATIVE_HH__

#include <vector>
#include "indexing_base.h"

//#include "params/SetAssociative.hh"

namespace Kuiper {
    namespace Cache {

        // class ReplaceableEntry;

        // /**
        //  * A set associative indexing policy.
        //  * @sa  \ref gem5MemorySystem "gem5 Memory System"
        //  *
        //  * The set associative indexing policy has an immutable/identity mapping, so a
        //  * value x is always mapped to set x, independent of the way, that is,
        //  * Hash(A, 0) = Hash(A, 1) = Hash(A, N-1), where N is the number of ways.
        //  *
        //  * For example, let's assume address A maps to set 3 on way 0. This policy
        //  * makes so that A is also mappable to set 3 on every other way. Visually, the
        //  * possible locations of A are, for a table with 4 ways and 8 sets:
        //  *    Way 0   1   2   3
        //  *  Set   _   _   _   _
        //  *    0  |_| |_| |_| |_|
        //  *    1  |_| |_| |_| |_|
        //  *    2  |_| |_| |_| |_|
        //  *    3  |X| |X| |X| |X|
        //  *    4  |_| |_| |_| |_|
        //  *    5  |_| |_| |_| |_|
        //  *    6  |_| |_| |_| |_|
        //  *    7  |_| |_| |_| |_|
        //  */
        // class SetAssociative : public BaseIndexingPolicy
        // {
        // protected:
        //     /**
        //      * Apply a hash function to calculate address set.
        //      *
        //      * @param addr The address to calculate the set for.
        //      * @return The set index for given combination of address and way.
        //      */
        //     virtual uint32_t extractSet(const Addr addr) const;

        // public:
        //     /**
        //      * Convenience typedef.
        //      */
        //     typedef SetAssociativeParams Params;

        //     /**
        //      * Construct and initialize this policy.
        //      */
        //     SetAssociative(const Params& p);

        //     /**
        //      * Destructor.
        //      */
        //     ~SetAssociative() {};

        //     /**
        //      * Find all possible entries for insertion and replacement of an address.
        //      * Should be called immediately before ReplacementPolicy's findVictim()
        //      * not to break cache resizing.
        //      * Returns entries in all ways belonging to the set of the address.
        //      *
        //      * @param addr The addr to a find possible entries for.
        //      * @return The possible entries.
        //      */
        //     std::vector<ReplaceableEntry*> getPossibleEntries(const Addr addr) const
        //         override;

        //     /**
        //      * Regenerate an entry's address from its tag and assigned set and way.
        //      *
        //      * @param tag The tag bits.
        //      * @param entry The entry.
        //      * @return the entry's original addr value.
        //      */
        //     Addr regenerateAddr(const Addr tag, const ReplaceableEntry* entry) const
        //         override;
        // };
    } // namespace Cache
} // namespace Kuipr

#endif //__MEM_CACHE_INDEXING_POLICIES_SET_ASSOCIATIVE_HH__
