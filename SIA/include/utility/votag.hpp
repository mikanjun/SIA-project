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
        t<=t;
        t=t;
    };

    template <typename Tag, size_t Size, typename Indices, typename ValueType>
    struct votag_base;
    template <typename Tag, size_t Size, size_t... Indices, typename ValueType>
    struct votag_base<Tag, Size, std::index_sequence<Indices...>, ValueType>
    {
        std::pair<Tag, ValueType> m_data[Size];

        // constexpr votag_base()                             noexcept = delete;
        constexpr votag_base(const votag_base&)            noexcept = default;
        // constexpr votag_base(votag_base&&)                 noexcept = delete;
        constexpr votag_base& operator=(const votag_base&) noexcept = default;
        // constexpr votag_base& operator=(votag_base&&)      noexcept = delete;

        constexpr votag_base() noexcept : 
            m_data{{static_cast<Tag>(Indices), ValueType{ }}...}
        { }
        
        template <typename... TagArgs>
        constexpr votag_base(const TagArgs&... args) noexcept : 
            m_data{{static_cast<Tag>(Indices), ValueType{ }}...}
        {
            (++m_data[static_cast<size_t>(args)].second, ...);
        }

        // Duplicated input must be check by user.
        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr size_t count(const TagArgs&... args) const noexcept
        {
            ValueType ret{ };
            auto _lam_get = [&] (const size_t& idx)->ValueType {return m_data[idx].second;};
            ((ret += _lam_get(static_cast<size_t>(args))), ...);
            return ret;
        }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr bool query(const TagArgs&... args) const noexcept
        {
            auto _lam_query = [&] (const size_t& idx)->bool {return static_cast<bool>(m_data[idx].second);};
            return (_lam_query(static_cast<size_t>(args)) | ...);
        }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr void insert(const TagArgs&... args) noexcept
        {
            auto _lam_inc = [&] (const size_t& idx)->void {++m_data[idx].second;};
            (_lam_inc(static_cast<size_t>(args)), ...);
        }

        constexpr void insert(const Tag& arg, const ValueType& val) noexcept
        {
            m_data[static_cast<size_t>(arg)].second += val;
        }

        // template <typename... TagArgs>
        //     requires (std::is_same_v<Tag, TagArgs> && ...)
        // constexpr bool remove(const TagArgs&... args) noexcept
        // {
        //     ValueType _tras[Size]{ };
        //     auto _lam_gather = [&] (const size_t& idx)->void {++_tras[idx];};
        //     auto _lam_checker = [&] (const size_t& idx)->bool {return _tras[idx] <= m_data[idx].second;};
        //     auto _lam_proc = [&] (const size_t& idx) {m_data[idx].second -= _tras[idx];};
        //     (_lam_gather(static_cast<size_t>(args)), ...);
        //     if((_lam_checker(Indices) || ...))
        //     {
        //         (_lam_proc(Indices), ...);
        //         return true;
        //     }
        //     else
        //     {
        //         return false;
        //     }
        // }

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
                    if(ValueType{ } == m_data[static_cast<size_t>(tag)].second)
                    {
                        _proc_guard = true;
                        return &tag;
                    }
                    else
                    {
                        --m_data[static_cast<size_t>(tag)].second;
                        return nullptr;
                    }
                }
                return nullptr;
            };
            bool _undo_guard{false};
            auto _lam_undo_proc = [&] (const Tag* p_arg, const Tag& tag)
            {
                if(!_undo_guard)
                {
                    if(p_arg == &tag)
                    {
                        _undo_guard = true;
                    }
                    else
                    {
                        ++m_data[static_cast<size_t>(tag)].second;
                    }
                }
            };
            auto _lam_undo = [&] (const Tag* p_arg)
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
                m_data[static_cast<size_t>(arg)].second -= val;
                return true;
            }
            return false;
        }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> && ...)
        constexpr void abandon(const TagArgs&... args) noexcept
        {
            auto _lam_set_zero = [&] (const size_t& idx)->void {m_data[idx].second = ValueType{ };};
            ((_lam_set_zero(static_cast<size_t>(args))), ...);
        }

        constexpr bool empty() const noexcept
        {
            bool ret{false};
            for(const auto& [tag, val] : m_data)
            {
                if(ret |= static_cast<bool>(val))
                {
                    return ret;
                }
            }
            return ret;
        }

        constexpr void reset() noexcept
        {
            for(auto& [tag, val] : m_data)
            {
                val = ValueType{ };
            }
        }

        constexpr void swap(const Tag& first, const Tag& second) noexcept
        {
            if(&first != &second)
            {
                const ValueType tmp{m_data[static_cast<size_t>(first)].second};
                m_data[static_cast<size_t>(first)].second = m_data[static_cast<size_t>(second)].second;
                m_data[static_cast<size_t>(second)].second = tmp;
            }
        }
    };

    template <typename Tag, size_t Size, typename ValueType = size_t>
        requires _votag_tag_requirement<Tag> && _votag_value_requirement<ValueType>
    struct votag : public votag_base<Tag, Size, std::make_index_sequence<Size>, ValueType>
    {        
        // constexpr votag()                        noexcept = delete;
        constexpr votag(const votag&)            noexcept = default;
        // constexpr votag(votag&&)                 noexcept = delete;
        constexpr votag& operator=(const votag&) noexcept = default;
        // constexpr votag& operator=(votag&&)      noexcept = delete;
        
        constexpr votag() noexcept :
            votag_base<Tag, Size, std::make_index_sequence<Size>, ValueType>{ }
        { }

        template <typename... TagArgs>
            requires (std::is_same_v<Tag, TagArgs> || ...)
        constexpr votag(const TagArgs&... args) noexcept : 
            votag_base<Tag, Size, std::make_index_sequence<Size>, ValueType>{args...}
        { }
    };
} // namespace sia