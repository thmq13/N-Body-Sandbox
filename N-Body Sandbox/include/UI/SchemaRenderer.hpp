#pragma once

namespace NBody::Core { struct ParameterSchema; }

namespace NBody::UI::SchemaRenderer {
    void drawInput(Core::ParameterSchema& schema, float fieldWidth = 100.0f);
    void drawDisplay(const Core::ParameterSchema& schema);
}
