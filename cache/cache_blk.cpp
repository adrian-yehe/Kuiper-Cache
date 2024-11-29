#include "cache/include/cache_blk.h"

namespace Kuiper {
    namespace Cache {

        void
            CacheBlk::insert(const Addr tag, const bool is_secure,
                const int src_requestor_ID, const uint32_t task_ID) {
            // Make sure that the block has been properly invalidated
            assert(!isValid());

            insert(tag, is_secure);

            // Set source requestor ID
            setSrcRequestorId(src_requestor_ID);

            // Set task ID
            setTaskId(task_ID);

            // Set insertion tick as current tick
            setTickInserted();

            // Insertion counts as a reference to the block
            increaseRefCount();
        }

        void
            CacheBlkPrintWrapper::print(std::ostream& os, int verbosity,
                const std::string& prefix) const {
            //ccprintf(os, "%sblk %c%c%c%c\n", prefix,
            //    blk->isValid() ? 'V' : '-',
            //    blk->isSet(CacheBlk::WritableBit) ? 'E' : '-',
            //    blk->isSet(CacheBlk::DirtyBit) ? 'M' : '-',
            //    blk->isSecure() ? 'S' : '-');
        }
    } // namespace Cache
} // namespace Kuiper
