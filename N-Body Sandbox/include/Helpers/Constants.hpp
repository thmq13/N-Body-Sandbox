#pragma once

#include <cstddef>
#include <limits>

namespace Limit {
    inline constexpr double doubleMax{ std::numeric_limits<double>::max() };
    inline constexpr double doubleMin{ std::numeric_limits<double>::lowest() };
    inline constexpr double doubleEpsilon{ std::numeric_limits<double>::epsilon() };

    inline constexpr double floatMax{ std::numeric_limits<float>::max() };
    inline constexpr double floatMin{ std::numeric_limits<float>::lowest() };
    inline constexpr double floatEpsilon{ std::numeric_limits<float>::epsilon() };

    inline constexpr int intMax { std::numeric_limits<int>::max() };
    inline constexpr int intMin{ std::numeric_limits<int>::lowest() };

    inline constexpr std::size_t sizeTMax{ std::numeric_limits<std::size_t>::max() };

}