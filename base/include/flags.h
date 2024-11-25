#ifndef  __CACHE_FLAGS_H__
#define __CACHE__FLAGS_H__

#include <type_traits>

namespace Kuiper {
    namespace Cache {
        /**
        * Wrapper that groups a few flag bits under the same undelying container.
        *
        * @tparam T The type of the underlying container. Must be an unsigned integer.
        */
        template <typename T>
        class Flags {
        private:
            static_assert(std::is_unsigned_v<T>, "Flag type must be unsigned");

            /** The undelying container of the flags' bits. */
            T _flags;

        public:
            typedef T Type;

            /**
             * @ingroup api_flags
             * @{
             */

             /**
              * Initialize flags with a given value. If no value is provided, the
              * flag bits are initialized cleared.
              *
              * @param flags The value to initialize the flags with.
              */
            Flags(Type flags = 0) : _flags(flags) {}

            operator const Type() const { return _flags; }

            const Flags<T>&
                operator=(T flags)
            {
                _flags = flags;
                return *this;
            }

            /**
             * Verifies whether any bit matching the given mask is set.
             *
             * @param mask The mask containing the bits to verify.
             * @return True if any matching bit is set; false otherwise.
             */
            bool isSet(Type mask) const { return (_flags & mask); }

            /**
             * Verifies whether no bits matching the given mask are set.
             *
             * @param mask The mask containing the bits to verify.
             * @return True if matching bits are set; false otherwise.
             */
            bool allSet(Type mask) const { return (_flags & mask) == mask; }

            /**
             * Verifies whether no bits matching the given mask are set.
             *
             * @param mask The mask containing the bits to verify.
             * @return True if matching bits are cleared; false otherwise.
             */
            bool noneSet(Type mask) const { return (_flags & mask) == 0; }

            /** Clear all flag's bits. */
            void clear() { _flags = 0; }

            /**
             * Clear all flag's bits matching the given mask.
             *
             * @param mask The mask containing the bits to be cleared.
             */
            void clear(Type mask) { _flags &= ~mask; }

            /**
             * Set all flag's bits matching the given mask.
             *
             * @param mask The mask containing the bits to be set.
             */
            void set(Type mask) { _flags |= mask; }

            /**
             * Conditionally set or clear some bits of the flag, given a mask.
             *
             * @param mask The mask containing the bits to be modified.
             * @param condition If true, set masked bits; otherwise, clear them.
             */
            void set(Type mask, bool condition) {
                condition ? set(mask) : clear(mask);
            }

            /**
             * Replace the contents of the bits matching the mask with the
             * corresponding bits in the provided flags.
             *
             * This is equivalent to:
             *     flags.clear(mask); flags.set(flags & mask);
             *
             * @param flags Flags to extract new bits from.
             * @param mask Mask used to determine which bits are replaced.
             */
            void replace(Type flags, Type mask) {
                _flags = (_flags & ~mask) | (flags & mask);
            }
            /** @} */ // end of api_flags
        };
    }  //namespace Cache
} //namespace Kuiper

#endif // ! __CACHE_FLAGS_H__

