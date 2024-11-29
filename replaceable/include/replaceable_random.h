#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_RANDOM_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_RANDOM_RP_HH__

#include "include/replaceable_base.h"

namespace Kuiper {
    namespace Cache {
        struct RandomRPParams;
        namespace replacement_policy {
           class Random : public Base {
           protected:
               /** Random-specific implementation of replacement data. */
               struct RandomReplData : ReplacementData {
                   /**
                    * Flag informing if the replacement data is valid or not.
                    * Invalid entries are prioritized to be evicted.
                    */
                   bool valid;

                   /**
                    * Default constructor. Invalidate data.
                    */
                   RandomReplData() : valid(false) {}
               };

           public:
               typedef RandomRPParams Params;
               Random(const Params& p);
               ~Random() = default;

               /**
                * Invalidate replacement data to set it as the next probable victim.
                * Prioritize replacement data for victimization.
                *
                * @param replacement_data Replacement data to be invalidated.
                */
               void invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
                   override;

               /**
                * Touch an entry to update its replacement data.
                * Does not do anything.
                *
                * @param replacement_data Replacement data to be touched.
                */
               void touch(const std::shared_ptr<ReplacementData>& replacement_data) const
                   override;

               /**
                * Reset replacement data. Used when an entry is inserted.
                * Unprioritize replacement data for victimization.
                *
                * @param replacement_data Replacement data to be reset.
                */
               void reset(const std::shared_ptr<ReplacementData>& replacement_data) const
                   override;

               /**
                * Find replacement victim at random.
                *
                * @param candidates Replacement candidates, selected by indexing policy.
                * @return Replacement entry to be replaced.
                */
               ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const
                   override;

               /**
                * Instantiate a replacement data entry.
                *
                * @return A shared pointer to the new replacement data.
                */
               std::shared_ptr<ReplacementData> instantiateEntry() override;
           };
        } // namespace replacement_policy
    } // namespace Cache 
} // namespace Kuiper

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_RANDOM_RP_HH__
