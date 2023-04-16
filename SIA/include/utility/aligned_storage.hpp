#pragma once

#include <type_traits>

#include "SIA/include/internals/types.hpp"

namespace sia
{    
    template <size_t Size, size_t Align, typename AlignType>
    union aligned_type
    {
        AlignType m_data;
        unsigned_byte_t<1> _m_pad[Size];

        template <typename Ret>
        constexpr Ret& as() noexcept
        {
            return *reinterpret_cast<Ret*>(this);
        }
    };

    template <typename size_t Size, size_t Align, size_t Target = 1>
    consteval auto _get_aligned_type() noexcept
    {
        if constexpr (alignof(unsigned_byte_t<Target>) == alignof(max_align_t))
        {
            return aligned_type<Size, Align, max_align_t>{ };
        }
        else if constexpr (Align == alignof(unsigned_byte_t<Target>))
        {
            return aligned_type<Size, Align, unsigned_byte_t<Target>>{ };
        }
        else
        {
            return _get_aligned_type<Size, Align, Target*2>();
        }
    }

    template <size_t Size, size_t Align>
    struct aligned_storage
    {
        using type = decltype(_get_aligned_type<Size, Align, 1>());
    };

    template <size_t Size, size_t Align>
    using aligned_storage_t = typename aligned_storage<Size, Align>::type;
    
} // namespace sia
