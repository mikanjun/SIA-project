#pragma once

#include <type_traits>
#include <utility>

#include "SIA/include/internals/types.hpp"

namespace sia
{
    template <typename T>
    concept _votag_tag_requirement = (is_scoped_enum_v<T> || std::is_enum_v<T>) || (std::is_integral_v<T> && std::is_convertible_v<T, size_t>);
    template <typename T>
    concept _votag_value_requirement = std::is_convertible_v<T, bool> && std::is_same_v<T, std::remove_cvref_t<T>>;

    template <typename T1, typename T2>
    constexpr std::pair<T1&, T2&> _votag_base_init_act(std::pair<T1&, T2&> target)
    {
        ++target.second;
        return target;
    }

    template <typename Tag, size_t Size, typename Indices, typename Value_t>
    struct votag_base;
    template <typename Tag, size_t Size, size_t... Indices, typename Value_t>
    struct votag_base<Tag, Size, std::index_sequence<Indices...>, Value_t>
    {
        std::pair<Tag, Value_t> m_data[Size];
        std::pair<Tag&, Value_t&> _init_actor;

        // constexpr votag_base()                             noexcept = delete;
        constexpr votag_base(const votag_base&)            noexcept = default;
        // constexpr votag_base(votag_base&&)                 noexcept = delete;
        constexpr votag_base& operator=(const votag_base&) noexcept = default;
        // constexpr votag_base& operator=(votag_base&&)      noexcept = delete;

        constexpr votag_base() noexcept : 
            m_data{{static_cast<Tag>(Indices), static_cast<Value_t>(0)}...},
            _init_actor{m_data[0].first, m_data[0].second}
        { }
        
        template <typename... TagArgs>
        constexpr votag_base(const TagArgs&... args) noexcept : 
            m_data{{static_cast<Tag>(Indices), static_cast<Value_t>(0)}...},
            _init_actor{(_votag_base_init_act<Tag, Value_t>({m_data[static_cast<size_t>(args)].first, m_data[static_cast<size_t>(args)].second}), ...)}
        { }

        template <typename... TagArgs>
        constexpr size_t count(const TagArgs&... args) const noexcept
        {
            size_t ret{ };
            ((ret += m_data[static_cast<size_t>(args)].second), ...);
            return ret;
        }

        template <typename... TagArgs>
        constexpr bool query(const TagArgs&... args) const noexcept
        {
            return static_cast<bool>(count(args...));
        }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr void insert(const TagArgs&... args) noexcept
        {
            ((++m_data[static_cast<size_t>(args)].second), ...);
        }

        constexpr void insert(const Tag& arg, const Value_t& val) noexcept
        {
            m_data[static_cast<size_t>(arg)].second += val;
        }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr void remove(const TagArgs&... args) noexcept
        {
            ((m_data[static_cast<size_t>(args)].second == static_cast<Value_t>(0) ? 0 : --m_data[static_cast<size_t>(args)].second), ...);
        }

        constexpr void remove(const Tag& arg, const Value_t& val) noexcept
        {
            if(m_data[static_cast<size_t>(arg)].second >= val)
            {
                m_data[static_cast<size_t>(arg)].second -= val;
            }
        }

        template <typename... TagArgs>
        constexpr void abandon(const TagArgs&... args) noexcept
        {
            ((m_data[static_cast<size_t>(args)].second = 0), ...);
        }

        constexpr bool empty() const noexcept
        {
            size_t ret{ };
            for(const auto& [tag, val] : m_data)
            {
                ret += val;
            }
            return static_cast<bool>(ret);
        }

        constexpr void reset() noexcept
        {
            for(auto& [tag, val] : m_data)
            {
                val = 0;
            }
        }

        constexpr void swap(const Tag& first, const Tag& second) noexcept
        {
            if(first != second)
            {
                const Value_t tmp{m_data[static_cast<size_t>(first)].second};
                m_data[static_cast<size_t>(first)].second = m_data[static_cast<size_t>(second)].second;
                m_data[static_cast<size_t>(second)].second = tmp;
            }
        }
    };

    template <typename Tag, size_t Size, typename Value_t = size_t>
        requires _votag_tag_requirement<Tag> && _votag_value_requirement<Value_t>
    struct votag : public votag_base<Tag, Size, std::make_index_sequence<Size>, Value_t>
    {        
        // constexpr votag()                        noexcept = delete;
        constexpr votag(const votag&)            noexcept = default;
        // constexpr votag(votag&&)                 noexcept = delete;
        constexpr votag& operator=(const votag&) noexcept = default;
        // constexpr votag& operator=(votag&&)      noexcept = delete;
        
        constexpr votag() noexcept :
            votag_base<Tag, Size, std::make_index_sequence<Size>, Value_t>{ }
        { }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> || ...)
        constexpr votag(const TagArgs&... args) noexcept : 
            votag_base<Tag, Size, std::make_index_sequence<Size>, Value_t>{args...}
        { }
    };
} // namespace sia