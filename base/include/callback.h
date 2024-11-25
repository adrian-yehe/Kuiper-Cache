#ifndef __BASE_CALLBACK_HH__
#define __BASE_CALLBACK_HH__

#include <functional>
#include <list>

namespace Kuiper {
    namespace Cache {
        class CallbackQueue : public std::list<std::function<void()>> {
        public:
            using Base = std::list<std::function<void()>>;

            using Base::Base;

            /**
             * @ingroup api_callback
             */
            void process() {
                for (auto& f : *this)
                    f();
            }
        };
    } // namespace Cache
} // namespace Kuiper

#endif // __BASE_CALLBACK_HH__
