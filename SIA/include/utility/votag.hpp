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
    concept _votag_value_requirement = requires(T t)
    {
        std::is_convertible_v<T, bool>;
        std::is_same_v<T, std::remove_cvref_t<T>>;
        T{};
        ++t;
        --t;
        t-t;
        t+t;
        t=t;
        t<=t;
    };

    template <typename Tag, size_t Size, typename ValueType>
    struct votag_impl
    {
        ValueType m_data[Size];

        // constexpr votag_impl()                             noexcept = delete;
        constexpr votag_impl(const votag_impl&)            noexcept = default;
        // constexpr votag_impl(votag_impl&&)                 noexcept = delete;
        constexpr votag_impl& operator=(const votag_impl&) noexcept = default;
        // constexpr votag_impl& operator=(votag_impl&&)      noexcept = delete;

        constexpr votag_impl() noexcept : 
            m_data{ValueType{ }...}
        { }
        
        template <typename... TagArgs>
        constexpr votag_impl(const TagArgs&... args) noexcept : 
            m_data{ }
        {
            (++m_data[static_cast<size_t>(args)], ...);
        }

        // Duplicated input must be check by user.
        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr ValueType count(const TagArgs&... args) const noexcept
        {
            ValueType ret{ };
            auto _lam_get = [&] (const size_t& idx)->ValueType {return m_data[idx];};
            ((ret += _lam_get(static_cast<size_t>(args))), ...);
            return ret;
        }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr bool query(const TagArgs&... args) const noexcept
        {
            size_t res {0};
            auto _lam_query = [&] (const size_t& idx)->void
            {
                if(static_cast<bool>(m_data[idx]))
                {
                    ++res;
                }
            };
            (_lam_query(static_cast<size_t>(args)), ...);
            return res == sizeof...(args);
        }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr void insert(const TagArgs&... args) noexcept
        {
            auto _lam_inc = [&] (const size_t& idx)->void {++m_data[idx];};
            (_lam_inc(static_cast<size_t>(args)), ...);
        }

        constexpr void insert(const Tag& arg, const ValueType& val) noexcept
        {
            m_data[static_cast<size_t>(arg)] += val;
        }

        // Return true if success
        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr bool remove(const TagArgs&... args) noexcept
        {
            bool _proc_guard{false};
            auto _lam_proc = [&] (const Tag& tag)->const Tag*
            {
                if(!_proc_guard)
                {
                    if(ValueType{ } == m_data[static_cast<size_t>(tag)])
                    {
                        _proc_guard = true;
                        return &tag;
                    }
                    else
                    {
                        --m_data[static_cast<size_t>(tag)];
                        return nullptr;
                    }
                }
                return nullptr;
            };
            bool _undo_guard{false};
            auto _lam_undo_proc = [&] (const Tag* p_arg, const Tag& tag)->void
            {
                if(!_undo_guard)
                {
                    if(p_arg == &tag)
                    {
                        _undo_guard = true;
                    }
                    else
                    {
                        ++m_data[static_cast<size_t>(tag)];
                    }
                }
            };
            auto _lam_undo = [&] (const Tag* p_arg)->void
            {
                if(!_undo_guard)
                {
                    if(p_arg != nullptr)
                    {
                        (_lam_undo_proc(p_arg, args), ...);
                    }
                }
            };
            (_lam_undo(_lam_proc(args)), ...);
            return !_undo_guard;
        }

        // Return true if success
        constexpr bool remove(const Tag& arg, const ValueType& val) noexcept
        {
            if(val <= m_data[static_cast<size_t>(arg)].second)
            {
                m_data[static_cast<size_t>(arg)] -= val;
                return true;
            }
            return false;
        }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr void abandon(const TagArgs&... args) noexcept
        {
            auto _lam_set_zero = [&] (const size_t& idx)->void {m_data[idx] = ValueType{ };};
            ((_lam_set_zero(static_cast<size_t>(args))), ...);
        }

        constexpr bool empty() const noexcept
        {
            for(const auto& val : m_data)
            {
                if(static_cast<bool>(val))
                {
                    return false;
                }
            }
            return true;
        }

        constexpr void reset() noexcept
        {
            for(auto& val : m_data)
            {
                val = ValueType{ };
            }
        }

        constexpr void swap(const Tag& first, const Tag& second) noexcept
        {
            if(first != second)
            {
                const ValueType tmp{m_data[static_cast<size_t>(first)]};
                m_data[static_cast<size_t>(first)] = m_data[static_cast<size_t>(second)];
                m_data[static_cast<size_t>(second)] = tmp;
            }
        }
    };

    template <typename Tag, size_t Size, typename ValueType = size_t>
        requires _votag_tag_requirement<Tag> && _votag_value_requirement<ValueType>
    struct votag : public votag_impl<Tag, Size, ValueType>
    {        
        // constexpr votag()                        noexcept = delete;
        constexpr votag(const votag&)            noexcept = default;
        // constexpr votag(votag&&)                 noexcept = delete;
        constexpr votag& operator=(const votag&) noexcept = default;
        // constexpr votag& operator=(votag&&)      noexcept = delete;
        
        constexpr votag() noexcept :
            votag_impl<Tag, Size, ValueType>{ }
        { }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> || ...)
        constexpr votag(const TagArgs&... args) noexcept : 
            votag_impl<Tag, Size, ValueType>{args...}
        { }
    };
} // namespace sia