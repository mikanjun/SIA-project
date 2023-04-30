#pragma once

#include "SIA/include/internals/types.hpp"
#include "SIA/include/type_traits/type_traits.hpp"

#include <tuple>

namespace sia
{
    template <typename... TagTypes>
        requires (is_scoped_enum_v<TagTypes> && ...)
    struct solitag
    {
        using tuple_type = std::tuple<TagTypes...>;
        std::tuple<TagTypes...> m_data;

        constexpr solitag()                          noexcept = delete;
        constexpr solitag(const solitag&)            noexcept = default;
        // constexpr solitag(solitag&&)                 noexcept = delete;
        constexpr solitag& operator=(const solitag&) noexcept = default;
        // constexpr solitag& operator=(solitag&&)      noexcept = delete
        constexpr solitag(const TagTypes&... args) noexcept : m_data{std::make_tuple(args...)} { }
        template <typename... Types>
            requires (is_scoped_enum_v<Types> && ...)
        constexpr solitag(const std::tuple<Types...>& arg) noexcept : m_data{arg} { }

        template <typename... Types>
        constexpr bool query(const Types&... args) const noexcept
        {
            size_t res{0};
            auto _lam_proc = [&]<size_t... Indices> (const auto& arg, std::index_sequence<Indices...>)->void
            {
                if(((std::get<Indices>(m_data) == arg) || ...))
                {
                    ++res;
                }
            };
            (_lam_proc(args, std::make_index_sequence<sizeof...(TagTypes)>{ }), ...);
            return res == sizeof...(args);
        }
        
        template <size_t Pos, typename Type>
        constexpr void set(const Type& arg) noexcept
        {
            std::get<Pos>(m_data) = arg;
        }

        constexpr auto tuple() const noexcept
        {
            tuple_type ret {m_data};
            return ret;
        }
    };
} // namespace sia