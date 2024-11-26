#ifndef BASE_TYPETRAITS_HH
#define BASE_TYPETRAITS_HH

#include <iostream>
#include <tuple>
#include <type_traits>

namespace Kuiper {
    namespace Cache {
        template<typename F>
        struct MemberFunctionSignature;
        template<typename C, typename R, class... A>
        struct MemberFunctionSignature<R(C::*)(A...)>
        {
            using class_t = C;
            using return_t = R;
            using argsTuple_t = std::tuple<A...>;
        };
        template<typename C, typename R, class... A>
        struct MemberFunctionSignature<R(C::*)(A...) const>
        {
            using class_t = std::add_const_t<C>;
            using return_t = R;
            using argsTuple_t = std::tuple<A...>;
        };
        template<typename C, typename R, class... A>
        struct MemberFunctionSignature<R(C::*)(A...) volatile>
        {
            using class_t = std::add_volatile_t<C>;
            using return_t = R;
            using argsTuple_t = std::tuple<A...>;
        };
        template<typename C, typename R, class... A>
        struct MemberFunctionSignature<R(C::*)(A...) const volatile>
        {
            using class_t = std::add_cv_t<C>;
            using return_t = R;
            using argsTuple_t = std::tuple<A...>;
        };
        template<auto F>
        using MemberFunctionClass_t =
            typename MemberFunctionSignature<decltype(F)>::class_t;

        template<auto F>
        using MemberFunctionReturn_t =
            typename MemberFunctionSignature<decltype(F)>::return_t;

        template<auto F>
        using MemberFunctionArgsTuple_t =
            typename MemberFunctionSignature<decltype(F)>::argsTuple_t;

        // iterable type trait
        template <typename, typename = void>
        struct is_iterable : std::false_type {};

        template <typename T>
        struct is_iterable<T,
            std::void_t<decltype(begin(std::declval<T>())),
            decltype(end(std::declval<T>()))>> : std::true_type {};

        template <typename T>
        constexpr bool is_iterable_v = is_iterable<T>::value;

        // std::hash-enabled type trait
        template <typename, typename = void>
        struct is_std_hash_enabled : std::false_type {};

        template <typename T>
        struct is_std_hash_enabled<T,
            std::void_t<decltype(std::hash<T>())>> : std::true_type {};

        template <typename T>
        constexpr bool is_std_hash_enabled_v = is_std_hash_enabled<T>::value;
    } // namespace Cache
} // namespace Kuiper

#endif // BASE_TYPETRAITS_HH
