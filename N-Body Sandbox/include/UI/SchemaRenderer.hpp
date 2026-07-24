#pragma once

namespace NBody::Core { struct ParameterSchema; }

namespace NBody::UI::SchemaRenderer {

    bool DrawInput(Core::ParameterSchema& schema, float fieldWidth = 100.0f);
    void DrawDisplay(const Core::ParameterSchema& schema);

} // namespace NBody::UI::SchemaRenderer
