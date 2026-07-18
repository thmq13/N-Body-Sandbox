#pragma once

#include <string>
#include <unordered_map>
#include <array>

#include <Core/Message.hpp>

namespace NBody::UI {
    using UIStoredTargets = std::unordered_map<std::string, std::vector<Core::ParameterSchema>>;
    using UIStoredSubModules = std::vector<UIStoredTargets>;
    using UIStoredModules = std::array<UIStoredSubModules, Core::moduleCount>;
    using UIStorage = UIStoredModules;

    using UIActiveTarget = std::array<std::vector<std::string>, Core::moduleCount>;
}


