#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_BASE_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_BASE_HH__

#include <memory>

// #include "base/compiler.hh"
#include "replaceable_entry.h"
#include "cache_packet.h"
// #include "params/BaseReplacementPolicy.hh"
// #include "sim/sim_object.hh"

namespace Kuiper {
    namespace Cache {
        // /**
        // * Replacement candidates as chosen by the indexing policy.
        // */
        // typedef std::vector<ReplaceableEntry*> ReplacementCandidates;
        // namespace replacement_policy {
        //     /**
        //      * A common base class of cache replacement policy objects.
        //      */
        //     class Base {
        //     public:
        //         typedef BaseReplacementPolicyParams Params;
        //         Base(const Params &p) : SimObject(p) {}
        //         virtual ~Base() = default;

        //         /**
        //          * Invalidate replacement data to set it as the next probable victim.
        //          *
        //          * @param replacement_data Replacement data to be invalidated.
        //          */
        //         virtual void invalidate(const std::shared_ptr<ReplacementData>&
        //             replacement_data) = 0;

        //         /**
        //          * Update replacement data.
        //          *
        //          * @param replacement_data Replacement data to be touched.
        //          * @param pkt Packet that generated this access.
        //          */
        //         virtual void touch(const std::shared_ptr<ReplacementData>&
        //             replacement_data, const PacketPtr pkt) {
        //             touch(replacement_data);
        //         }
        //         virtual void touch(const std::shared_ptr<ReplacementData>&
        //             replacement_data) const = 0;

        //         /**
        //          * Reset replacement data. Used when it's holder is inserted/validated.
        //          *
        //          * @param replacement_data Replacement data to be reset.
        //          * @param pkt Packet that generated this access.
        //          */
        //         virtual void reset(const std::shared_ptr<ReplacementData>&
        //             replacement_data, const PacketPtr pkt) {
        //             reset(replacement_data);
        //         }
        //         virtual void reset(const std::shared_ptr<ReplacementData>&
        //             replacement_data) const = 0;

        //         /**
        //          * Find replacement victim among candidates.
        //          *
        //          * @param candidates Replacement candidates, selected by indexing policy.
        //          * @return Replacement entry to be replaced.
        //          */
        //         virtual ReplaceableEntry* getVictim(
        //                             const ReplacementCandidates& candidates) const = 0;

        //         /**
        //          * Instantiate a replacement data entry.
        //          *
        //          * @return A shared pointer to the new replacement data.
        //          */
        //         virtual std::shared_ptr<ReplacementData> instantiateEntry() = 0;
        //     };
        // } // namespace replacement_policy
    } // namespace Cache 
} // namespace Kuiper 

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_BASE_HH__
