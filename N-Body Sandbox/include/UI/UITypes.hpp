#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

#include <Core/Message.hpp>

namespace NBody::UI {

    using UIStoredTargets = std::unordered_map<std::string, std::vector<Core::ParameterSchema>>;
    using UIStoredSubModules = std::vector<UIStoredTargets>;
    using UIStoredModules = std::vector<UIStoredSubModules>;
    using UIStorage = UIStoredModules;

    using UIActiveTarget = std::vector<std::string>;

} // namespace NBody::UI


