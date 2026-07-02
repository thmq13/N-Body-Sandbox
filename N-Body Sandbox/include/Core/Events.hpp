#pragma once

#include <variant>
#include <string>



#include <UI/UIState.hpp>

struct ComputeConfig {
    enum class Algorithm {DirectN2, BarnesHut};
    enum class Hardware {CPU, GPU};

    Algorithm algorithm{ Algorithm::DirectN2 };
    Hardware hardware{ Hardware::CPU };
    bool useOpenMP = true;
    bool useSIMD = true;
};

struct PhysicsConfig {
    double GConstant{ 1.0 };
    double deltaTime{ 1.0 / 60.0 };
    double softeningLength{ 0.5 };
    double theta = 0.5;
};

struct SpanwerConfig {
    enum class Shape { UniformSphere, Plummer, DiskGalaxy };
    
    Shape shape{ Shape::UniformSphere };
   //....
};

struct CmdExitApplication {};

struct CmdRequestUIStateChange {
    UIState requestedState;
};

struct CmdUpdatePreview {

};

struct EvtSimulationError {
    std::string moduleName;
    std::string errorMessage;
};

using SystemMessage = std::variant <
    CmdExitApplication,
    CmdRequestUIStateChange,
    EvtSimulationError
>;