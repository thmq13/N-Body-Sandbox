#include <Core/Application.hpp>
#include <Core/Logger.hpp>

int main() {
    NBody::Core::Application app{};

    auto initRes{ app.InitializeSubsystems() };
    if (!initRes) {
        NBODY_FATAL("ENTRY: Failed to initialize app: {}", initRes.error());
        return 1;
    }

    auto loopRes{ app.ExecuteMasterLoop() };
    if (!loopRes) {
        NBODY_FATAL("ENTRY: Failed to enter master loop : {}", loopRes.error());
        return 1;
    }

    return 0;
}