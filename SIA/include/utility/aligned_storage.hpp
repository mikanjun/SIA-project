#pragma once

#include <type_traits>

#include "SIA/include/internals/types.hpp"

namespace sia
{    
    template <size_t Size, size_t Align, typename Aligned_t>
    union aligned_type
    {
        Aligned_t m_data;
        char _m_pad[Size];

        template <typename Ret>
        constexpr Ret& as() noexcept
        {
            return *reinterpret_cast<Ret*>(this);
        } 
    };

    template <size_t Size, size_t Align>
    consteval auto aligned_storage_impl() noexcept
    {
        if constexpr (Align <= alignof(char))
        {
            return aligned_type<Size, Align, char>{ };
        }
        else if constexpr (Align <= alignof(short))
        {
            return aligned_type<Size, Align, short>{ };
        }
        else if constexpr (Align <= alignof(int))
        {
            return aligned_type<Size, Align, int>{ };
        }
        else
        {
            return aligned_type<Size, Align, double>{ };
        }
    }

    template <size_t Size, size_t Align>
    struct aligned_storage
    {
        using type = decltype(aligned_storage_impl<Size, Align>());
    };

    template <size_t Size, size_t Align>
    using aligned_storage_t = typename aligned_storage<Size, Align>::type;
    
} // namespace sia
