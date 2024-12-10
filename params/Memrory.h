#ifndef __MEMORY_HH_
#define __MEMORY_HH_

#include <cstdint>
#include <string>

namespace Kuiper {
    namespace Cache {
        struct MemoryParams{
           std::string name; 
           std::uint32_t read_delay;
           std::uint32_t write_delay;
           std::uint64_t cap;
           std::uint32_t width;
           std::uint32_t id;
        };
    }
}

#endif /* __MEMORY_HH_ */
