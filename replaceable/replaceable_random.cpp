#include <cassert>
#include <memory>
#include "random.h"
#include "include/replaceable_random.h"
//#include "params/RandomRP.h"

namespace Kuiper {
    namespace Cache {
        namespace replacement_policy {

            Random::Random() {};

            void
            Random::invalidate(const std::shared_ptr<ReplacementData> &replacement_data) {
                // Unprioritize replacement data victimization
                std::static_pointer_cast<RandomReplData>(replacement_data)->valid = false;
            }

            void
            Random::touch(const std::shared_ptr<ReplacementData> &replacement_data) const
            {
            }

            void
            Random::reset(const std::shared_ptr<ReplacementData> &replacement_data) const {
                // Unprioritize replacement data victimization
                std::static_pointer_cast<RandomReplData>(
                    replacement_data)
                    ->valid = true;
            }

            ReplaceableEntry *
            Random::getVictim(const ReplacementCandidates &candidates) const {
                // There must be at least one replacement candidate
                assert(candidates.size() > 0);

                // Choose one candidate at random
                ReplaceableEntry *victim = candidates[gem5::random_mt.random<unsigned>(0,
                                                                                 candidates.size() - 1)];

                // Visit all candidates to search for an invalid entry. If one is found,
                // its eviction is prioritized
                for (const auto &candidate : candidates) {
                    if (!std::static_pointer_cast<RandomReplData>(candidate->replacementData) ->valid) {
                        victim = candidate;
                        break;
                    }
                }

                return victim;
            }

            std::shared_ptr<ReplacementData>
            Random::instantiateEntry() {
                return std::shared_ptr<ReplacementData>(new RandomReplData());
            }
        } // namespace replacement_policy
    } // namespace Cache
} // namespace Kuiper
