/*
 * Copyright (c) 2014 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2003-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Mersenne twister random number generator.
 */

#ifndef __BASE_RANDOM_HH__
#define __BASE_RANDOM_HH__

#include <random>
#include <string>
#include <type_traits>

// #include "base/compiler.hh"
#include "types.h"
// #include "sim/serialize.hh"

namespace gem5
{

class Checkpoint;

// class Random : public Serializable
class Random
{

  public:

    /**
     * @ingroup api_base_utils
     */
    std::mt19937_64 gen;

    /**
     * @ingroup api_base_utils
     * @{
     */
    Random();
    Random(uint32_t s);
    /** @} */ // end of api_base_utils
    ~Random();

    void init(uint32_t s);

    /**
     * Use the SFINAE idiom to choose an implementation based on
     * whether the type is integral or floating point.
     *
     * @ingroup api_base_utils
     */
    template <typename T>
    typename std::enable_if_t<std::is_integral_v<T>, T>
    random()
    {
        // [0, max_value] for integer types
        std::uniform_int_distribution<T> dist;
        return dist(gen);
    }

    /**
     * @ingroup api_base_utils
     */
    template <typename T>
    typename std::enable_if_t<std::is_floating_point_v<T>, T>
    random()
    {
        // [0, 1) for real types
        std::uniform_real_distribution<T> dist;
        return dist(gen);
    }
    /**
     * @ingroup api_base_utils
     */
    template <typename T>
    typename std::enable_if_t<std::is_integral_v<T>, T>
    random(T min, T max)
    {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(gen);
    }

    // void serialize(CheckpointOut &cp) const override;
    // void unserialize(CheckpointIn &cp) override;
};

/**
 * @ingroup api_base_utils
 */
extern Random random_mt;

} // namespace gem5

#endif // __BASE_RANDOM_HH__
