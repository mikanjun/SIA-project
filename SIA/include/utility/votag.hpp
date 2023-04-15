#pragma once

#include <type_traits>
#include <utility>

#include "SIA/include/internals/types.hpp"
#include "SIA/include/type_traits/type_traits.hpp"

namespace sia
{
    template <typename T>
    concept _votag_tag_requirement = (is_scoped_enum_v<T> || std::is_enum_v<T>) || (std::is_integral_v<T> && std::is_convertible_v<T, size_t>);
    template <typename T>
    concept _votag_value_requirement = std::is_convertible_v<T, bool> && std::is_same_v<T, std::remove_cvref_t<T>>;

    template <typename First, typename Second, typename Idx, typename... TagArgs>
    constexpr std::pair<First, Second> _init_act(const Idx& idx, const TagArgs&... args) noexcept
    {
        std::pair<First, Second> ret{static_cast<First>(idx), static_cast<Second>(0)};
        ((ret.first == args ? ++ret.second : 0), ...);
        return ret;
    }

    template <typename Tag, size_t Size, typename Indices, typename Value>
    struct votag_base;
    template <typename Tag, size_t Size, size_t... Indices, typename Value>
    struct votag_base<Tag, Size, std::index_sequence<Indices...>, Value>
    {
        std::pair<Tag, Value> m_data[Size];

        // constexpr votag_base()                             noexcept = delete;
        constexpr votag_base(const votag_base&)            noexcept = default;
        // constexpr votag_base(votag_base&&)                 noexcept = delete;
        constexpr votag_base& operator=(const votag_base&) noexcept = default;
        // constexpr votag_base& operator=(votag_base&&)      noexcept = delete;

        constexpr votag_base() noexcept : 
            m_data{{static_cast<Tag>(Indices), static_cast<Value>(0)}...}
        { }
        
        template <typename... TagArgs>
        constexpr votag_base(const TagArgs&... args) noexcept : 
            m_data{_init_act<Tag, Value>(Indices, args...)...}
        { }

        template <typename... TagArgs>
        constexpr size_t count(const TagArgs&... args) const noexcept
        {
            size_t ret{ };
            auto _lam_get = [&] (const Tag& idx)->size_t {return static_cast<size_t>(m_data[static_cast<size_t>(idx)].second);};
            ((ret += _lam_get(args)), ...);
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
            auto _lam_inc = [&] (const Tag& idx)->void {++m_data[static_cast<size_t>(idx)].second;};
            ((_lam_inc(args)), ...);
        }

        constexpr void insert(const Tag& arg, const Value& val) noexcept
        {
            m_data[static_cast<size_t>(arg)].second += val;
        }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr void remove(const TagArgs&... args) noexcept
        {
            Value _tras[Size]{ };
            bool _block{false};
            auto _lam_proc = [&] (const bool& is_zero, const size_t& idx)->void
            {
                if(!_block){
                    if(is_zero)
                    {
                        _block = true;
                        for(auto p_tras = _tras; auto& [tag, val] : m_data)
                        {
                            val += *p_tras;
                            ++p_tras;
                        }
                    }
                    else
                    {
                        ++_tras[idx];
                        --m_data[idx].second;
                    }
                }
            };
            ((_lam_proc(m_data[static_cast<size_t>(args)].second == 0, static_cast<size_t>(args))), ...);
        }

        constexpr void remove(const Tag& arg, const Value& val) noexcept
        {
            if(m_data[static_cast<size_t>(arg)].second >= val)
            {
                m_data[static_cast<size_t>(arg)].second -= val;
            }
        }

        template <typename... TagArgs>
        constexpr void abandon(const TagArgs&... args) noexcept
        {
            auto _lam_set_zero = [&] (const size_t& idx)->void {m_data[idx].second = 0;};
            ((_lam_set_zero(static_cast<size_t>(args))), ...);
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
                const Value tmp{m_data[static_cast<size_t>(first)].second};
                m_data[static_cast<size_t>(first)].second = m_data[static_cast<size_t>(second)].second;
                m_data[static_cast<size_t>(second)].second = tmp;
            }
        }
    };

    template <typename Tag, size_t Size, typename Value = size_t>
        requires _votag_tag_requirement<Tag> && _votag_value_requirement<Value>
    struct votag : public votag_base<Tag, Size, std::make_index_sequence<Size>, Value>
    {        
        // constexpr votag()                        noexcept = delete;
        constexpr votag(const votag&)            noexcept = default;
        // constexpr votag(votag&&)                 noexcept = delete;
        constexpr votag& operator=(const votag&) noexcept = default;
        // constexpr votag& operator=(votag&&)      noexcept = delete;
        
        constexpr votag() noexcept :
            votag_base<Tag, Size, std::make_index_sequence<Size>, Value>{ }
        { }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> || ...)
        constexpr votag(const TagArgs&... args) noexcept : 
            votag_base<Tag, Size, std::make_index_sequence<Size>, Value>{args...}
        { }
    };
} // namespace sia