#pragma once

#include <type_traits>

#include "SIA/include/internals/types.hpp"

namespace sia
{
    template <typename T, bool B = std::is_enum_v<T>>
    struct is_scoped_enum : std::false_type { };
    template <typename T>
    struct is_scoped_enum<T, true> : std::bool_constant<!std::is_convertible_v<T, std::underlying_type_t<T>>> { };
    template <typename T>
    inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;
} // namespace sia
