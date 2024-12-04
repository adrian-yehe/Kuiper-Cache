#include <string>
#include "intmath.h"
#include "include/tag_set_assoc.h"

namespace Kuiper {
    namespace Cache {
        BaseSetAssoc::BaseSetAssoc(const Params &p) : 
            BaseTags(p), 
            allocAssoc(p.assoc), 
            blks(p.size / p.block_size),
            sequentialAccess(p.sequential_access),
            replacementPolicy(p.replacement_policy) {
            // There must be a indexing policy
            assert(p.indexing_policy && "An indexing policy is required");
          
            // Check parameters
            assert((blkSize < 4 || isPowerOf2(blkSize)) && 
                    "Block size must be at least 4 and a power of 2");
            
            assert(p.replacement_policy);
        }

        void
        BaseSetAssoc::tagsInit() {
            // Initialize all blocks
            for (unsigned blk_index = 0; blk_index < numBlocks; blk_index++) {
                // Locate next cache block
                CacheBlk *blk = &blks[blk_index];

                // Link block to indexing policy
                indexingPolicy->setEntry(blk, blk_index);

                // Associate a data chunk to the block
                blk->data = &dataBlks[blkSize * blk_index];

                // Associate a replacement data entry to the block
                blk->replacementData = replacementPolicy->instantiateEntry();
            }
        }

        void
        BaseSetAssoc::invalidate(CacheBlk *blk)
        {
            BaseTags::invalidate(blk);

            // Decrease the number of tags in use
            // stats.tagsInUse--;

            // Invalidate replacement data
            replacementPolicy->invalidate(blk->replacementData);
        }

        void
        BaseSetAssoc::moveBlock(CacheBlk *src_blk, CacheBlk *dest_blk)
        {
            BaseTags::moveBlock(src_blk, dest_blk);

            // Since the blocks were using different replacement data pointers,
            // we must touch the replacement data of the new entry, and invalidate
            // the one that is being moved.
            replacementPolicy->invalidate(src_blk->replacementData);
            replacementPolicy->reset(dest_blk->replacementData);
        }
    } // namespace Cache
} // namespace Kuiper
