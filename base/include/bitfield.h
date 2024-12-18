#ifndef __BASE_BITFIELD_HH__
#define __BASE_BITFIELD_HH__

#include <bitset>
#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace Kuiper {
    namespace Cache {

        extern const uint8_t reverseBitsLookUpTable[];

        /**
         * Generate a 64-bit mask of 'nbits' 1s, right justified. If a number of bits
         * greater than 64 is given, it is truncated to 64.
         *
         * @param nbits The number of bits set in the mask.
         *
         * @ingroup api_bitfield
         */
        constexpr uint64_t
            mask(unsigned nbits)
        {
            return (nbits >= 64) ? (uint64_t)-1LL : (1ULL << nbits) - 1;
        }

        /**
         * Extract the bitfield from position 'first' to 'last' (inclusive)
         * from 'val' and right justify it.  MSB is numbered 63, LSB is 0.
         *
         * @ingroup api_bitfield
         */
        template <class T>
        constexpr T
            bits(T val, unsigned first, unsigned last)
        {
            assert(first >= last);
            int nbits = first - last + 1;
            return (val >> last) & mask(nbits);
        }

        /**
         * Extract the bit from this position from 'val' and right justify it.
         *
         * @ingroup api_bitfield
         */
        template <class T>
        constexpr T
            bits(T val, unsigned bit)
        {
            return bits(val, bit, bit);
        }

        /**
         * Mask off the given bits in place like bits() but without shifting.
         * msb = 63, lsb = 0
         *
         * @ingroup api_bitfield
         */
        template <class T>
        constexpr T
            mbits(T val, unsigned first, unsigned last)
        {
            return val & (mask(first + 1) & ~mask(last));
        }

        /**
         * @ingroup api_bitfield
         */
        constexpr uint64_t
            mask(unsigned first, unsigned last)
        {
            return mbits((uint64_t)-1LL, first, last);
        }

        /**
         * Sign-extend an N-bit value to 64 bits. Assumes all bits past the sign are
         * currently zero. For true sign extension regardless of the value of the sign
         * bit, see szext.
         *
         * @ingroup api_bitfield
         */
        template <int N>
        constexpr uint64_t
            sext(uint64_t val)
        {
            bool sign_bit = bits(val, N - 1);
            if (sign_bit)
                val |= ~mask(N);
            return val;
        }

        /**
         * Sign-extend an N-bit value to 64 bits. Assumes all bits past the sign are
         * currently zero. For true sign extension regardless of the value of the sign
         * bit, see szext.
         *
         * @ingroup api_bitfield
         */
        constexpr uint64_t
            sext(uint64_t val, int N)
        {
            bool sign_bit = bits(val, N - 1);
            if (sign_bit)
                val |= ~mask(N);
            return val;
        }

        /**
         * Sign-extend an N-bit value to 64 bits. Zero any bits past the sign if
         * necessary.
         *
         * @ingroup api_bitfield
         */
        template <int N>
        constexpr uint64_t
            szext(uint64_t val)
        {
            bool sign_bit = bits(val, N - 1);
            if (sign_bit)
                val |= ~mask(N);
            else
                val &= mask(N);
            return val;
        }

        /**
         * Returns val with bits first to last set to the LSBs of bit_val
         *
         * E.g.:
         * first: 7
         * last:  4
         * val:      0xFFFF
         * bit_val:  0x0000
         * returned: 0xFF0F
         *
         * @ingroup api_bitfield
         */
        template <class T, class B>
        constexpr T
            insertBits(T val, unsigned first, unsigned last, B bit_val)
        {
            assert(first >= last);
            T bmask = mask(first, last);
            val &= ~bmask;
            val |= ((T)bit_val << last) & bmask;
            return val;
        }

        /**
         * Overloaded for access to only one bit in value
         *
         * @ingroup api_bitfield
         */
        template <class T, class B>
        constexpr T
            insertBits(T val, unsigned bit, B bit_val)
        {
            return insertBits(val, bit, bit, bit_val);
        }

        /**
         * A convenience function to replace bits first to last of val with bit_val
         * in place. It is functionally equivalent to insertBits.
         *
         * \note "first" is the MSB and "last" is the LSB. "first" >= "last"
         *
         * @ingroup api_bitfield
         */
        template <class T, class B>
        constexpr void
            replaceBits(T& val, unsigned first, unsigned last, B bit_val)
        {
            val = insertBits(val, first, last, bit_val);
        }

        /**
         * Overloaded function to allow to access only 1 bit
         *
         * @ingroup api_bitfield
         */
        template <class T, class B>
        constexpr void
            replaceBits(T& val, unsigned bit, B bit_val)
        {
            val = insertBits(val, bit, bit, bit_val);
        }

        /**
         * Takes a value and returns the bit reversed version.
         *
         * E.g.:
         * val:      0x0303
         * returned: 0xc0c0
         *
         * val:      0x0303
         * size:     1
         * returned: 0x03c0
         *
         * Algorithm from:
         * http://graphics.stanford.edu/~seander/bithacks.html#ReverseBitsByLookupTable
         *
         * @param val: variable length value
         * @param size: number of bytes to mirror
         * @return reversed value
         *
         * @ingroup api_bitfield
         */
        template <class T>
        std::enable_if_t<std::is_integral_v<T>, T>
            reverseBits(T val, size_t size = sizeof(T))
        {
            assert(size <= sizeof(T));

            if  (sizeof(T) == 1) {
                return reverseBitsLookUpTable[val];
            }
            else {
                T output = {};

                for (size_t byte = 0; byte < size; byte++) {
                    output = (output << 8) | reverseBitsLookUpTable[val & mask(8)];
                    val >>= 8;
                }

                return output;
            }
        }

        /**
         * Returns the bit position of the MSB that is set in the input
         *
         * @ingroup api_bitfield
         */
        constexpr int
            findMsbSet(uint64_t val)
        {
            int msb = 0;
            if (!val)
                return 0;
            if (bits(val, 63, 32)) {
                msb += 32;
                val >>= 32;
            }
            if (bits(val, 31, 16)) {
                msb += 16;
                val >>= 16;
            }
            if (bits(val, 15, 8)) {
                msb += 8;
                val >>= 8;
            }
            if (bits(val, 7, 4)) {
                msb += 4;
                val >>= 4;
            }
            if (bits(val, 3, 2)) {
                msb += 2;
                val >>= 2;
            }
            if (bits(val, 1, 1))
                msb += 1;
            return msb;
        }

        namespace {
            template<typename T>
            constexpr bool
                hasBuiltinCtz() {
                // Since the defined(__has_builtin) in the subsequent #if statement
                // won't short-circuit the macro expansion of
                // __has_builtin(__builtin_ctz), we must explicitly define it as zero
                // if it's undefined to avoid a preprocessor error.
#ifndef __has_builtin
#   define __has_builtin(foo) 0
#endif
#if defined(__has_builtin) && __has_builtin(__builtin_ctz)
                return sizeof(unsigned long long) >= sizeof(T);
#else
                return false;
#endif
            }

            constexpr int
                findLsbSetFallback(uint64_t val) {
                int lsb = 0;
                if (!val) {
                    return sizeof(val) * 8;
                }
                if (!bits(val, 31, 0)) {
                    lsb += 32;
                    val >>= 32;
                }
                if (!bits(val, 15, 0)) {
                    lsb += 16;
                    val >>= 16;
                }
                if (!bits(val, 7, 0)) {
                    lsb += 8;
                    val >>= 8;
                }
                if (!bits(val, 3, 0)) {
                    lsb += 4;
                    val >>= 4;
                }
                if (!bits(val, 1, 0)) {
                    lsb += 2;
                    val >>= 2;
                }
                if (!bits(val, 0, 0)) {
                    lsb += 1;
                }
                return lsb;
            }
        } // anonymous namespace

        /**
         * Returns the bit position of the LSB that is set in the input
         * That function will either use a builtin that exploit a "count trailing
         * zeros" instruction or use fall back method, `findLsbSetFallback`.
         *
         * @ingroup api_bitfield
         */
        constexpr int
            findLsbSet(uint64_t val) {
            if (val == 0) return 64;

            if constexpr (hasBuiltinCtz<decltype(val)>()) {
                return __builtin_ctzll(val);
            }
            else {
                return findLsbSetFallback(val);
            }
        }


        template<size_t N>
        constexpr int
            findLsbSet(std::bitset<N> bs)
        {
            if  (N <= 64) {
                return findLsbSet(bs.to_ullong());
            }
            else {
                if (bs.none()) return N;
                // Mask of ones
                constexpr std::bitset<N> mask(std::numeric_limits<uint64_t>::max());
                // Is the lsb set in the rightmost 64 bits ?
                auto nextQword{ bs & mask };
                int i{ 0 };
                while (nextQword.none()) {
                    // If no, shift by 64 bits and repeat
                    i += 64;
                    bs >>= 64;
                    nextQword = bs & mask;
                }
                // If yes, account for the bumber of 64-bit shifts and add the
                // remaining using the uint64_t implementation. Store in intermediate
                // variable to ensure valid conversion from ullong to uint64_t.
                uint64_t remaining{ nextQword.to_ullong() };
                return i + findLsbSet(remaining);
            }
        }

        /**
         * Returns the number of set ones in the provided value.
         * PD algorithm from
         * http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
         *
         * @ingroup api_bitfield
         */
        constexpr int
            popCount(uint64_t val)
        {
#ifndef __has_builtin
#   define __has_builtin(foo) 0
#endif
#if defined(__GNUC__) || \
    (defined(__clang__) && __has_builtin(__builtin_popcountl))
            return __builtin_popcountl(val);
#else
            const uint64_t m1 = 0x5555555555555555ULL;  // ..010101b
            const uint64_t m2 = 0x3333333333333333ULL;  // ..110011b
            const uint64_t m4 = 0x0f0f0f0f0f0f0f0fULL;  // ..001111b
            const uint64_t sum = 0x0101010101010101ULL;

            val -= (val >> 1) & m1;               // 2 bits count -> 2 bits
            val = (val & m2) + ((val >> 2) & m2); // 4 bits count -> 4 bits
            val = (val + (val >> 4)) & m4;        // 8 bits count -> 8 bits
            return (val * sum) >> 56;             // horizontal sum
#endif // defined(__GNUC__) ||
            //(defined(__clang__) && __has_builtin(__builtin_popcountl))
        }

        /**
         * Align to the next highest power of two.
         *
         * The number passed in is aligned to the next highest power of two,
         * if it is not already a power of two. Please note that if 0 is
         * passed in, 0 is returned.
         *
         * This code has been modified from the following:
         * http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
         *
         * @ingroup api_bitfield
         */
        constexpr uint64_t
            alignToPowerOfTwo(uint64_t val)
        {
            val--;
            val |= val >> 1;
            val |= val >> 2;
            val |= val >> 4;
            val |= val >> 8;
            val |= val >> 16;
            val |= val >> 32;
            val++;

            return val;
        };

        /**
         * Count trailing zeros in a 32-bit value.
         *
         * @param An input value
         * @return The number of trailing zeros or 32 if the value is zero.
         *
         * @ingroup api_bitfield
         */
        constexpr int
            ctz32(uint32_t value)
        {
            return value ? __builtin_ctzl(value) : 32;
        }

        /**
         * Count trailing zeros in a 64-bit value.
         *
         * @param An input value
         * @return The number of trailing zeros or 64 if the value is zero.
         *
         * @ingroup api_bitfield
         */
        constexpr int
            ctz64(uint64_t value)
        {
            return value ? __builtin_ctzll(value) : 64;
        }

        /**
         * Count leading zeros in a 32-bit value.
         *
         * @param An input value
         * @return The number of trailing zeros or 32 if the value is zero.
         *
         * @ingroup api_bitfield
         */
        constexpr inline int
            clz32(uint32_t value)
        {
            return value ? __builtin_clz(value) : 32;
        }

        /**
         * Count leading zeros in a 64-bit value.
         *
         * @param An input value
         * @return The number of trailing zeros or 64 if the value is zero.
         *
         * @ingroup api_bitfield
         */
        constexpr inline int
            clz64(uint64_t value)
        {
            return value ? __builtin_clzll(value) : 64;
        }
    } // namespace Cache
} // namespace Kuiper

#endif // __BASE_BITFIELD_HH__
