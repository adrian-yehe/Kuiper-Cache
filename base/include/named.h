#ifndef __NAMED_H__
#define __NAMED_H__

#include <string>

namespace Kuiper {
    namespace Cache {
        /** Interface for things with names. This is useful when using DPRINTF. */
        class Named {
        private:
            const std::string _name;

        public:
            Named(const std::string &name_) : _name(name_) {}
            virtual ~Named() = default;

            virtual std::string name() const { return _name; }
        };
    }
}

#endif /* __NAMED_H__ */




