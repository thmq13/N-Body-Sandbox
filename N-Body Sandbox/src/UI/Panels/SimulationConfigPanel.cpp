#include "UI/Panels/SimulationConfigPanel.hpp"

#include <algorithm>
#include <string>
#include <variant>
#include <type_traits>
#include <format>

#include <imgui.h>

#include <Core/Events.hpp>
#include <Core/MessageBus.hpp>

// =============================================================================
// Constants Management Convention
// =============================================================================
namespace {
    namespace SimulationConfigConfig {
        constexpr ImGuiDataType size_t_type = (sizeof(size_t) == 8) ? ImGuiDataType_U64 : ImGuiDataType_U32;

        constexpr ImGuiWindowFlags windowFlags{
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus
        };

        constexpr float widthScale{ 1.00f };       
        constexpr float heightScale{ 0.30f };      
        constexpr float offsetX{ 0.0f };

        constexpr float defaultFontScale{ 1.75f };
        constexpr float configFontScale{ 2.0f };

        constexpr float algoHWComboWidthScale{ 0.55f };
        constexpr float inteComboWidthScale{ 0.55f };
        constexpr float shapeComboWidthScale{ 0.30f };
        constexpr float frameBorderSize{ 1.0f };

        constexpr ImVec4 pureBlack{ 0.0f, 0.0f, 0.0f, 1.0f };
        constexpr ImVec4 mutedGray{ 0.2f, 0.2f, 0.2f, 1.0f };
        constexpr ImVec4 activeGray{ 0.3f, 0.3f, 0.3f, 1.0f };
        constexpr ImVec4 borderWhite{ 1.0f, 1.0f, 1.0f, 0.5f };
        constexpr ImVec4 retroWhite{ 1.0f, 1.0f, 1.0f, 1.0f };

        constexpr int styleColorCount{ 11 };

        constexpr const char* windowName{ "Simulation Configuration Panel" };
        constexpr const char* computeHeader{ "== COMPUTE CONFIG ==" };
        constexpr const char* physicsHeader{ "== PHYSICS CONFIG ==" };
        constexpr const char* spawnerHeader{ "== SPAWNER CONFIG ==" };
        constexpr const char* activeClustersHeader{ "== ACTIVE CLUSTERS ==" };
        constexpr const char* applyButtonText{ "[ APPLY AND START SIMULATION ]" };
        constexpr const char* addClusterButtonText{ "> Add Cluster <" };
        constexpr const char* noClustersText{ "No active clusters." };
        constexpr const char* removeClusterGlyph{ "X" };

        constexpr const char* algoLabel{ "Algorithm" };
        constexpr const char* hwLabel{ "Hardware" };
        constexpr const char* integratorLabel{ "Integrator" };
        constexpr const char* gConstantLabel{ "G Constant" };
        constexpr const char* deltaTimeLabel{ "Delta Time" };
        constexpr const char* softeningLengthLabel{ "Softening" };
        constexpr const char* thetaLabel{ "Theta" };
        constexpr const char* shapeLabel{ "Shape" };
        constexpr const char* radiusLabel{ "Radius" };
        constexpr const char* totalMassLabel{ "Total Mass" };
        constexpr const char* bodyCountLabel{ "Body Count" };
        constexpr const char* scaleRadiusLabel{ "Scale Radius" };
        constexpr const char* thicknessLabel{ "Thickness" };

        constexpr const char* algorithms{ "Direct O(N^2)\0Barnes-Hut\0" };
        constexpr const char* hardwareOptions{ "CPU\0GPU\0" };
        constexpr const char* integrators{ "Euler\0Verlet\0RK4\0" };
        constexpr const char* shapes{ "Unifrom Sphere\0Plummer\0Disk Galaxy\0" };

        constexpr const char* highPrecisionFormat{ "%.6f" };
        constexpr const char* medPrecisionFormat{ "%.4f" };

        std::string describeCluster(const SpawnerConfig::ClusterConfig& cluster) {
            return std::visit([](auto& params) -> std::string {
                using T = std::decay_t<decltype(params)>;

                if constexpr (std::is_same_v<T, SpawnerConfig::UniformSphereParams>) {
                    return std::format(
                        "Uniform Sphere:\n -- radius={:.2f}, mass={:.2f}, body count={}", 
                        params.radius, params.totalMass, params.bodyCount
                    );
                }
                else if constexpr (std::is_same_v<T, SpawnerConfig::PlummerParams>) {
                    return std::format(
                        "Plummer:\n -- scale radius={:.2f}, mass={:.2f}, body count={}", 
                        params.scaleRadius, params.totalMass, params.bodyCount
                    );
                } 
                else if constexpr (std::is_same_v<T, SpawnerConfig::DiskGalaxyParams>) {
                    return std::format(
                        "Disk Galaxy:\n -- radius={:.2f}, thickness={:.2f}, mass={:.2f}, body count={}",
                        params.radius, params.thickness, params.totalMass, params.bodyCount
                    );
                }
            }, cluster.params);
        }
    }
}

void SimulationConfigPanel::draw(MessageBus& messageBus) {
    setUpWindowAndStyle();

    const float windowWidth = ImGui::GetContentRegionAvail().x;

    ImGui::Columns(3, "ConfigColumns", true);
    ImGui::SetColumnWidth(0, windowWidth / 5.0);
    ImGui::SetColumnWidth(1, windowWidth / 5.0);

    drawComputeSection();
    ImGui::NextColumn();

    drawPhysicsSection();
    ImGui::NextColumn();

    drawSpawnerSection(messageBus);
    ImGui::Columns(1);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    drawApplyAndRunButton(messageBus);

    cleanUp();
}

void SimulationConfigPanel::setUpWindowAndStyle() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    const float windowWidth = viewport->Size.x * SimulationConfigConfig::widthScale;
    const float windowHeight = viewport->Size.y * SimulationConfigConfig::heightScale;
    const float posY = viewport->Pos.y + (viewport->Size.y - windowHeight);

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + SimulationConfigConfig::offsetX, posY));
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 15.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, SimulationConfigConfig::frameBorderSize);

    ImGui::Begin(SimulationConfigConfig::windowName, nullptr, SimulationConfigConfig::windowFlags);

    ImGui::PopStyleVar(2);

    ImGui::PushStyleColor(ImGuiCol_Text, SimulationConfigConfig::retroWhite);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, SimulationConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, SimulationConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, SimulationConfigConfig::activeGray);
    ImGui::PushStyleColor(ImGuiCol_Border, SimulationConfigConfig::borderWhite);
    ImGui::PushStyleColor(ImGuiCol_Button, SimulationConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SimulationConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, SimulationConfigConfig::activeGray);
    ImGui::PushStyleColor(ImGuiCol_Header, SimulationConfigConfig::pureBlack);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, SimulationConfigConfig::mutedGray);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, SimulationConfigConfig::activeGray);
}

void SimulationConfigPanel::cleanUp() {
    

    ImGui::SetWindowFontScale(SimulationConfigConfig::defaultFontScale);
    ImGui::PopStyleColor(SimulationConfigConfig::styleColorCount);
    ImGui::End();
}

void SimulationConfigPanel::drawComputeSection() {
    ImGui::SetWindowFontScale(SimulationConfigConfig::configFontScale);
    ImGui::Text(SimulationConfigConfig::computeHeader);
    ImGui::SetWindowFontScale(SimulationConfigConfig::defaultFontScale);
    ImGui::Spacing();

    float totalWidth = ImGui::GetContentRegionAvail().x;
    ImGui::PushItemWidth(totalWidth * SimulationConfigConfig::algoHWComboWidthScale);

    int currentAlgo = static_cast<int>(m_localComputeConfig.algorithm);
    if (ImGui::Combo(SimulationConfigConfig::algoLabel, &currentAlgo, SimulationConfigConfig::algorithms)) {
        m_localComputeConfig.algorithm = static_cast<ComputeConfig::Algorithm>(currentAlgo);
    }

    int currentHW = static_cast<int>(m_localComputeConfig.hardware);
    if (ImGui::Combo(SimulationConfigConfig::hwLabel, &currentHW, SimulationConfigConfig::hardwareOptions)) {
        m_localComputeConfig.hardware = static_cast<ComputeConfig::Hardware>(currentHW);
    }
    ImGui::PopItemWidth();
    ImGui::Spacing();

    if (m_localComputeConfig.hardware == ComputeConfig::Hardware::CPU) {
        ImGui::Checkbox("> Use OpenMP", &m_localComputeConfig.useOpenMP);
        ImGui::Checkbox("> Use SIMD", &m_localComputeConfig.useSIMD);
    }
    if (m_localComputeConfig.hardware == ComputeConfig::Hardware::GPU) {
        m_localComputeConfig.useOpenMP = false;
        m_localComputeConfig.useSIMD = false;
    }
}

void SimulationConfigPanel::drawPhysicsSection() {
    ImGui::SetWindowFontScale(SimulationConfigConfig::configFontScale);
    ImGui::Text(SimulationConfigConfig::physicsHeader);
    ImGui::SetWindowFontScale(SimulationConfigConfig::defaultFontScale);
    ImGui::Spacing();

    float totalWidth = ImGui::GetContentRegionAvail().x;
    ImGui::PushItemWidth(totalWidth * SimulationConfigConfig::inteComboWidthScale);

    int currentIntegrator = static_cast<int>(m_localPhysicsConfig.integrator);
    if (ImGui::Combo(SimulationConfigConfig::integratorLabel, &currentIntegrator, SimulationConfigConfig::integrators)) {
        m_localPhysicsConfig.integrator = static_cast<PhysicsConfig::Integrator>(currentIntegrator);
    }
    ImGui::Spacing();

    ImGui::InputScalar(SimulationConfigConfig::gConstantLabel, ImGuiDataType_Double, &m_localPhysicsConfig.GConstant, nullptr, nullptr, SimulationConfigConfig::highPrecisionFormat);
    ImGui::InputScalar(SimulationConfigConfig::deltaTimeLabel, ImGuiDataType_Double, &m_localPhysicsConfig.deltaTime, nullptr, nullptr, SimulationConfigConfig::highPrecisionFormat);
    ImGui::InputScalar(SimulationConfigConfig::softeningLengthLabel, ImGuiDataType_Double, &m_localPhysicsConfig.softeningLength, nullptr, nullptr, SimulationConfigConfig::highPrecisionFormat);

    if (m_localComputeConfig.algorithm == ComputeConfig::Algorithm::BarnesHut) {
        ImGui::InputScalar(SimulationConfigConfig::thetaLabel, ImGuiDataType_Double, &m_localPhysicsConfig.theta, nullptr, nullptr, SimulationConfigConfig::highPrecisionFormat);
    }
    ImGui::PopItemWidth();

    m_localPhysicsConfig.GConstant = std::max(0.0, m_localPhysicsConfig.GConstant);
    m_localPhysicsConfig.deltaTime = std::max(0.0, m_localPhysicsConfig.deltaTime);
    m_localPhysicsConfig.softeningLength = std::max(0.0, m_localPhysicsConfig.softeningLength);
    m_localPhysicsConfig.theta = std::max(0.0, m_localPhysicsConfig.theta);
}

void SimulationConfigPanel::drawSpawnerSection(MessageBus& messageBus) {
    ImGui::SetWindowFontScale(SimulationConfigConfig::configFontScale);
    ImGui::Text(SimulationConfigConfig::spawnerHeader);
    ImGui::SetWindowFontScale(SimulationConfigConfig::defaultFontScale);
    ImGui::Spacing();

    float totalWidth = ImGui::GetContentRegionAvail().x;
    ImGui::PushItemWidth(totalWidth * SimulationConfigConfig::shapeComboWidthScale);

    int currentShape = static_cast<int>(m_pendingShape);
    if (ImGui::Combo(SimulationConfigConfig::shapeLabel, &currentShape, SimulationConfigConfig::shapes)) {
        m_pendingShape = static_cast<SpawnerConfig::Shape>(currentShape);
        switch (m_pendingShape) {
        case SpawnerConfig::Shape::UniformSphere: m_pendingParams = SpawnerConfig::UniformSphereParams{}; break;
        case SpawnerConfig::Shape::Plummer:       m_pendingParams = SpawnerConfig::PlummerParams{};        break;
        case SpawnerConfig::Shape::DiskGalaxy:    m_pendingParams = SpawnerConfig::DiskGalaxyParams{};      break;
        }
    }
    ImGui::Spacing();

    std::visit([this](auto& params) {
        using T = std::decay_t<decltype(params)>;

        if constexpr (std::is_same_v<T, SpawnerConfig::UniformSphereParams>) {
            ImGui::InputScalar(SimulationConfigConfig::radiusLabel, ImGuiDataType_Double, &params.radius, nullptr, nullptr, SimulationConfigConfig::medPrecisionFormat);
            ImGui::InputScalar(SimulationConfigConfig::totalMassLabel, ImGuiDataType_Double, &params.totalMass,nullptr, nullptr, SimulationConfigConfig::medPrecisionFormat);
            ImGui::InputScalar(SimulationConfigConfig::bodyCountLabel, SimulationConfigConfig::size_t_type, &params.bodyCount);
            params.radius = std::max(0.0, params.radius);
            params.totalMass = std::max(0.0, params.totalMass);
        }
        else if constexpr (std::is_same_v<T, SpawnerConfig::PlummerParams>) {
            ImGui::InputScalar(SimulationConfigConfig::scaleRadiusLabel, ImGuiDataType_Double, &params.scaleRadius, nullptr, nullptr, SimulationConfigConfig::medPrecisionFormat);
            ImGui::InputScalar(SimulationConfigConfig::totalMassLabel, ImGuiDataType_Double, &params.totalMass, nullptr, nullptr, SimulationConfigConfig::medPrecisionFormat);
            ImGui::InputScalar(SimulationConfigConfig::bodyCountLabel, SimulationConfigConfig::size_t_type, &params.bodyCount);
            params.scaleRadius = std::max(0.0, params.scaleRadius);
            params.totalMass = std::max(0.0, params.totalMass);
        }
        else if constexpr (std::is_same_v<T, SpawnerConfig::DiskGalaxyParams>) {
            ImGui::InputScalar(SimulationConfigConfig::radiusLabel, ImGuiDataType_Double, &params.radius, nullptr, nullptr, SimulationConfigConfig::medPrecisionFormat);
            ImGui::InputScalar(SimulationConfigConfig::thicknessLabel, ImGuiDataType_Double, &params.thickness, nullptr, nullptr, SimulationConfigConfig::medPrecisionFormat);
            ImGui::InputScalar(SimulationConfigConfig::totalMassLabel, ImGuiDataType_Double, &params.totalMass, nullptr, nullptr, SimulationConfigConfig::medPrecisionFormat);
            ImGui::InputScalar(SimulationConfigConfig::bodyCountLabel, SimulationConfigConfig::size_t_type, &params.bodyCount);
            params.radius = std::max(0.0, params.radius);
            params.thickness = std::max(0.0, params.thickness);
            params.totalMass = std::max(0.0, params.totalMass);
        }
        params.bodyCount = std::max(0, static_cast<int>(params.bodyCount));

    }, m_pendingParams);
    ImGui::PopItemWidth();
    ImGui::Spacing();

    if (ImGui::Button(SimulationConfigConfig::addClusterButtonText, ImVec2(-1, 0))) {
        SpawnerConfig::ClusterConfig newCluster{ m_nextClusterId++, m_pendingParams };
        m_localSpawnerConfig.clusters.push_back(newCluster);
        messageBus.publish(CmdSpawnPreviewCluster{ newCluster });
    }
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextDisabled(SimulationConfigConfig::activeClustersHeader);

    if (!m_localSpawnerConfig.clusters.empty()) {
        for (auto it = m_localSpawnerConfig.clusters.begin(); it != m_localSpawnerConfig.clusters.end(); ) {
            ImGui::PushID(static_cast<int>(it->id));
            ImGui::TextUnformatted(SimulationConfigConfig::describeCluster(*it).c_str());
            ImGui::SameLine();

            if (ImGui::SmallButton(SimulationConfigConfig::removeClusterGlyph)) {
                messageBus.publish(CmdRemovePreviewCluster{it->id});
                it = m_localSpawnerConfig.clusters.erase(it);
            }
            else {
                ++it;
            }


            ImGui::PopID();
        }
    }
    else {
        ImGui::TextDisabled(SimulationConfigConfig::noClustersText);
    }
}

void SimulationConfigPanel::drawApplyAndRunButton(MessageBus& messageBus) {
    if (ImGui::Button(SimulationConfigConfig::applyButtonText, ImVec2(-1, 40))) {
        CmdUpdateSimulationConfig configCmd{
            m_localComputeConfig,
            m_localPhysicsConfig,
            m_localSpawnerConfig
        };
        messageBus.publish(configCmd);
        messageBus.publish(CmdRequestStateChange{ m_targetRunningState });
    }
}
