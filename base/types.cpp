#include <iostream>
#include "types.h"

namespace Kuiper {
    namespace Cache {

        std::ostream&
            operator<<(std::ostream& out, const Cycles& cycles)
        {
            out << cycles.c;
            return out;
        }
    } // namespace Cache
} // namespace Kuiper
