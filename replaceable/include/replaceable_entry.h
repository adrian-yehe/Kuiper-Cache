#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_REPLACEABLE_ENTRY_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_REPLACEABLE_ENTRY_HH__

#include <cstdint>
#include <memory>
#include <iostream>

namespace Kuiper {
    namespace Cache {
        namespace replacement_policy {
            /**
             * The replacement data needed by replacement policies. Each replacement policy
             * should have its own implementation of replacement data.
             */
            struct ReplacementData {};

        } // namespace replacement_policy

        /**
         * A replaceable entry is a basic entry in a 2d table-like structure that needs
         * to have replacement functionality. This entry is located in a specific row
         * and column of the table (set and way in cache nomenclature), which are
         * stored within the entry itself.
         *
         * It contains the replacement data pointer, which must be instantiated by the
         * replacement policy before being used.
         * @sa Replacement Policies
         */
        class ReplaceableEntry {
        protected:
            /**
             * Set to which this entry belongs.
             */
            uint32_t _set;

            /**
             * Way (relative position within the set) to which this entry belongs.
             */
            uint32_t _way;

        public:
            ReplaceableEntry() = default;
            virtual ~ReplaceableEntry() = default;

            /**
             * Replacement data associated to this entry.
             * It must be instantiated by the replacement policy before being used.
             */
            std::shared_ptr<replacement_policy::ReplacementData> replacementData;

            /**
             * Set both the set and way. Should be called only once.
             *
             * @param set The set of this entry.
             * @param way The way of this entry.
             */
            virtual void
                setPosition(const uint32_t set, const uint32_t way)
            {
                _set = set;
                _way = way;
            }

            /**
             * Get set number.
             *
             * @return The set to which this entry belongs.
             */
            uint32_t getSet() const { return _set; }

            /**
             * Get way number.
             *
             * @return The way to which this entry belongs.
             */
            uint32_t getWay() const { return _way; }

            /**
             * Prints relevant information about this entry.
             *
             * @return A string containg the contents of this entry.
             */
            virtual std::string print() const {
                std::string info;
                info.resize(100);
                sprintf(&info[0], "set: %#x way: %#x", getSet(), getWay());
                return (info);
            }
        };
    } // namespace Cache
} // namespace Kuiper

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_REPLACEABLE_ENTRY_HH_
