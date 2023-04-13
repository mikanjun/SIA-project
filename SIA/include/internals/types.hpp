#pragma once

namespace sia
{
    using max_align_t = double;
    using size_t = decltype(sizeof(char));
    using ptrdiff_t = decltype(static_cast<char*>(0) - static_cast<char*>(0));
} // namespace sia
