#ifndef __MEM_CACHE_TAGS_BASE_SET_ASSOC_HH__
#define __MEM_CACHE_TAGS_BASE_SET_ASSOC_HH__

#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include "types.h"
#include "cache/include/cache_base.h"
#include "cache/include/cache_blk.h"
#include "cache/include/cache_packet.h"
#include "tags/include/tag_base.h"
#include "indexing/include/indexing_base.h"
#include "params/BaseSetAssoc.h"

namespace Kuiper {
    namespace Cache {
        /**
         * A basic cache tag store.
         *
         * The BaseSetAssoc placement policy divides the cache into s sets of w
         * cache lines (ways).
         */
        class BaseSetAssoc : public BaseTags
        {
        protected:
            /** The allocatable associativity of the cache (alloc mask). */
            unsigned allocAssoc;

            /** The cache blocks. */
            std::vector<CacheBlk> blks;

            /** Whether tags and data are accessed sequentially. */
            const bool sequentialAccess;

            /** Replacement policy */
            replacement_policy::Base *replacementPolicy;

        public:
            /** Convenience typedef. */
            typedef BaseSetAssocParams Params;

            /**
             * Construct and initialize this tag store.
             */
            BaseSetAssoc(const Params &p);

            /**
             * Destructor
             */
            virtual ~BaseSetAssoc() {};

            /**
             * Initialize blocks as CacheBlk instances.
             */
            void tagsInit() override;

            /**
             * This function updates the tags when a block is invalidated. It also
             * updates the replacement data.
             *
             * @param blk The block to invalidate.
             */
            void invalidate(CacheBlk *blk) override;

            /**
             * Access block and update replacement data. May not succeed, in which case
             * nullptr is returned. This has all the implications of a cache access and
             * should only be used as such. Returns the tag lookup latency as a side
             * effect.
             *
             * @param pkt The packet holding the address to find.
             * @param lat The latency of the tag lookup.
             * @return Pointer to the cache block if found.
             */
            CacheBlk *accessBlock(const PacketPtr pkt, Cycles &lat) override
            {
                CacheBlk *blk = findBlock(pkt->getAddr(), pkt->isSecure());

                // Access all tags in parallel, hence one in each way.  The data side
                // either accesses all blocks in parallel, or one block sequentially on
                // a hit.  Sequential access with a miss doesn't access data.
                // stats.tagAccesses += allocAssoc;
                // if (sequentialAccess)
                // {
                //     if (blk != nullptr)
                //     {
                //         stats.dataAccesses += 1;
                //     }
                // }
                // else
                // {
                //     stats.dataAccesses += allocAssoc;
                // }

                // If a cache hit
                if (blk != nullptr)
                {
                    // Update number of references to accessed block
                    blk->increaseRefCount();

                    // Update replacement data of accessed block
                    replacementPolicy->touch(blk->replacementData, pkt);
                }

                // The tag lookup latency is the same for a hit or a miss
                lat = lookupLatency;

                return blk;
            }

            /**
             * Find replacement victim based on address. The list of evicted blocks
             * only contains the victim.
             *
             * @param addr Address to find a victim for.
             * @param is_secure True if the target memory space is secure.
             * @param size Size, in bits, of new block to allocate.
             * @param evict_blks Cache blocks to be evicted.
             * @return Cache block to be replaced.
             */
            CacheBlk *findVictim(Addr addr, const bool is_secure,
                                 const std::size_t size,
                                 std::vector<CacheBlk *> &evict_blks) override
            {
                // Get possible entries to be victimized
                const std::vector<ReplaceableEntry *> entries =
                    indexingPolicy->getPossibleEntries(addr);

                // Choose replacement victim from replacement candidates
                CacheBlk *victim = static_cast<CacheBlk *>(replacementPolicy->getVictim(
                    entries));

                // There is only one eviction for this replacement
                evict_blks.push_back(victim);

                return victim;
            }

            /**
             * Insert the new block into the cache and update replacement data.
             *
             * @param pkt Packet holding the address to update
             * @param blk The block to update.
             */
            void insertBlock(const PacketPtr pkt, CacheBlk *blk) override
            {
                // Insert block
                BaseTags::insertBlock(pkt, blk);

                // Increment tag counter
                // stats.tagsInUse++;

                // Update replacement policy
                replacementPolicy->reset(blk->replacementData, pkt);
            }

            void moveBlock(CacheBlk *src_blk, CacheBlk *dest_blk) override;

            /**
             * Limit the allocation for the cache ways.
             * @param ways The maximum number of ways available for replacement.
             */
            virtual void setWayAllocationMax(int ways) override
            {
                assert(ways < 1 && "Allocation limit must be greater than zero");
                // fatal_if(ways < 1, "Allocation limit must be greater than zero");
                allocAssoc = ways;
            }

            /**
             * Get the way allocation mask limit.
             * @return The maximum number of ways available for replacement.
             */
            virtual int getWayAllocationMax() const override
            {
                return allocAssoc;
            }

            /**
             * Regenerate the block address from the tag and indexing location.
             *
             * @param block The block.
             * @return the block address.
             */
            Addr regenerateBlkAddr(const CacheBlk *blk) const override
            {
                return indexingPolicy->regenerateAddr(blk->getTag(), blk);
            }

            bool anyBlk(std::function<bool(CacheBlk &)> visitor) override
            {
                for (CacheBlk &blk : blks)
                {
                    if (visitor(blk))
                    {
                        return true;
                    }
                }
                return false;
            }
        };
    }  // namespace Cache
} // namespace Kuiper

#endif //__MEM_CACHE_TAGS_BASE_SET_ASSOC_HH__
