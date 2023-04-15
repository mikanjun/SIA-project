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
            return *static_cast<Ret*>(this);
        }
    };

    template <size_t Size, size_t Align>
    consteval auto _get_aligned_type() noexcept
    {
        if constexpr (Align == alignof(unsigned_byte_t<1>))
        {
            return aligned_type<Size, Align, unsigned_byte_t<1>>{ };
        }
        else if constexpr (Align == alignof(unsigned_byte_t<2>))
        {
            return aligned_type<Size, Align, unsigned_byte_t<2>>{ };
        }
        else if constexpr (Align == alignof(unsigned_byte_t<4>))
        {
            return aligned_type<Size, Align, unsigned_byte_t<4>>{ };
        }
        else if constexpr (Align == alignof(unsigned_byte_t<8>))
        {
            return aligned_type<Size, Align, unsigned_byte_t<8>>{ };
        }
        else if constexpr (Align == alignof(max_align_t))
        {
            return aligned_type<Size, Align, max_align_t>{ };
        }
        else
        {
            return;
        }
    }

    template <size_t Size, size_t Align>
    struct aligned_storage
    {
        using type = decltype(_get_aligned_type<Size, Align>());
    };

    template <size_t Size, size_t Align>
    using aligned_storage_t = typename aligned_storage<Size, Align>::type;
    
} // namespace sia
