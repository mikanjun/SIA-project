#pragma once

#include "SIA/include/internals/types.hpp"
#include "SIA/include/type_traits/type_traits.hpp"

#include <tuple>

namespace sia
{
    template <auto... Tags>
        requires (is_scoped_enum_v<decltype(Tags)> && ...)
    struct contag
    {
        constexpr contag()                         noexcept = default;
        constexpr contag(const contag&)            noexcept = delete;
        // constexpr contag(contag&&)                  noexcept = delete;
        constexpr contag& operator=(const contag&) noexcept = delete;
        // constexpr contag& operator=(contag&&)       noexcept = delete;

        template <typename... TagTypes>
        constexpr bool query(const TagTypes&... args) const noexcept
        {
            size_t res {0};
            auto _lam_proc = [&] (const auto& arg)->void
            {
                if(((Tags == arg) || ...))
                {
                    ++res;
                }
            };
            (_lam_proc(args), ...);
            return res == sizeof...(args);
        }

        constexpr auto tuple() const noexcept
        {
            return std::make_tuple(Tags...);
        }
    };
} // namespace sia
