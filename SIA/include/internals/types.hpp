#pragma once

namespace sia
{
    template <size_t Size>
    struct unsigned_chunk;
    template <size_t Size>
    struct signed_chunk;

    using max_align_t = double;
    using size_t = decltype(sizeof(char));
    using ptrdiff_t = decltype(static_cast<char*>(0) - static_cast<char*>(0));

    template <size_t Size, bool Signed = false>
    consteval auto _get_byte_type()
    {
        if constexpr (Signed)
        {
            if constexpr (Size == sizeof(signed char))
            {
                return static_cast<signed char>(0);
            }
            else if constexpr (Size == sizeof(signed short))
            {
                return static_cast<signed short>(0);
            }
            else if constexpr (Size == sizeof(signed int))
            {
                return static_cast<signed int>(0);
            }
            else if constexpr (Size == sizeof(signed long))
            {
                return static_cast<signed long>(0);
            }
            else if constexpr (Size == sizeof(signed long long))
            {
                return static_cast<signed long long>(0);
            }
            else
            {
                return signed_chunk<Size>{};
            }
        }
        else
        {
            if constexpr (Size == sizeof(unsigned char))
            {
                return static_cast<unsigned char>(0);
            }
            else if constexpr (Size == sizeof(unsigned short))
            {
                return static_cast<unsigned short>(0);
            }
            else if constexpr (Size == sizeof(unsigned int))
            {
                return static_cast<unsigned int>(0);
            }
            else if constexpr (Size == sizeof(unsigned long))
            {
                return static_cast<unsigned long>(0);
            }
            else if constexpr (Size == sizeof(unsigned long long))
            {
                return static_cast<unsigned long long>(0);
            }
            else
            {
                return unsigned_chunk<Size>{};
            }
        }
    }

    template <size_t Size>
    using unsigned_byte_t = decltype(_get_byte_type<Size, false>());
    template <size_t Size>
    using signed_byte_t = decltype(_get_byte_type<Size, true>());

    template <size_t Size>
    struct unsigned_chunk
    {
        unsigned_byte_t<1> m_data[Size];
    };
    template <size_t Size>
    struct signed_chunk
    {
        signed_byte_t<1> m_data[Size];
    };

    using byte = unsigned_byte_t<1>;
    using word = unsigned_byte_t<2>;
    using dword = unsigned_byte_t<4>;
    using qword = unsigned_byte_t<8>;
} // namespace sia