#ifndef __CACHE_TAGGED_ENTRY_HH__
#define __CACHE_TAGGED_ENTRY_HH__

#include <cassert>
#include "types.h"
#include "replaceable/include/replaceable_entry.h"

namespace Kuiper {
    namespace Cache {
        /**
         * A tagged entry is an entry containing a tag. Each tag is accompanied by a
         * secure bit, which informs whether it belongs to a secure address space.
         * A tagged entry's contents are only relevant if it is marked as valid.
         */
        class TaggedEntry : public ReplaceableEntry {
        public:
            TaggedEntry() : _valid(false), _secure(false), _tag(MaxAddr) {}
            ~TaggedEntry() = default;

            /**
             * Checks if the entry is valid.
             *
             * @return True if the entry is valid.
             */
            virtual bool isValid() const { return _valid; }

            /**
             * Check if this block holds data from the secure memory space.
             *
             * @return True if the block holds data from the secure memory space.
             */
            bool isSecure() const { return _secure; }

            /**
             * Get tag associated to this block.
             *
             * @return The tag value.
             */
            virtual Addr getTag() const { return _tag; }

            /**
             * Checks if the given tag information corresponds to this entry's.
             *
             * @param tag The tag value to compare to.
             * @param is_secure Whether secure bit is set.
             * @return True if the tag information match this entry's.
             */
            virtual bool
                matchTag(Addr tag, bool is_secure) const
            {
                return isValid() && (getTag() == tag) && (isSecure() == is_secure);
            }

            /**
             * Insert the block by assigning it a tag and marking it valid. Touches
             * block if it hadn't been touched previously.
             *
             * @param tag The tag value.
             */
            virtual void
                insert(const Addr tag, const bool is_secure)
            {
                setValid();
                setTag(tag);
                if (is_secure) {
                    setSecure();
                }
            }

            /** Invalidate the block. Its contents are no longer valid. */
            virtual void invalidate()
            {
                _valid = false;
                setTag(MaxAddr);
                clearSecure();
            }

            const char * 
                print() const override
            {
                std::string info;
                info.resize(100);
                sprintf(&info[0], "tag: %#lx secure: %d valid: %d | %s", getTag(),
                    isSecure(), isValid(), ReplaceableEntry::print());
                return (info.c_str());
            }

        protected:
            /**
             * Set tag associated to this block.
             *
             * @param tag The tag value.
             */
            virtual void setTag(Addr tag) { _tag = tag; }

            /** Set secure bit. */
            virtual void setSecure() { _secure = true; }

            /** Set valid bit. The block must be invalid beforehand. */
            virtual void
                setValid()
            {
                assert(!isValid());
                _valid = true;
            }

        private:
            /**
             * Valid bit. The contents of this entry are only valid if this bit is set.
             * @sa invalidate()
             * @sa insert()
             */
            bool _valid;

            /**
             * Secure bit. Marks whether this entry refers to an address in the secure
             * memory space. Must always be modified along with the tag.
             */
            bool _secure;

            /** The entry's tag. */
            Addr _tag;

            /** Clear secure bit. Should be only used by the invalidation function. */
            void clearSecure() { _secure = false; }
        };
    } /* namespace Cache */
} /*  namespace Kuiper */

#endif//__CACHE_TAGGED_ENTRY_HH__
