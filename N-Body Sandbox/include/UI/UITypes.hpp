#pragma once

#include <string>
#include <unordered_map>
#include <array>

#include <Core/Message.hpp>

using UIStoredTargets = std::unordered_map<std::string, std::vector<ParameterSchema>>;
using UIStoredSubModules = std::vector<UIStoredTargets>;
using UIStoredModules = std::array<UIStoredSubModules, moduleCount>;
using UIStorage = UIStoredModules;

using UIActiveTarget = std::array<std::vector<std::string>, moduleCount>;

