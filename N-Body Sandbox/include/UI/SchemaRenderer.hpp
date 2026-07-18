#pragma once

namespace NBody::Core { struct ParameterSchema; }

namespace NBody::UI {
    namespace SchemaRenderer {
        void drawInput(Core::ParameterSchema& schema, float fieldWidth = 100.0f);
        void drawDisplay(const Core::ParameterSchema& schema);
    }
}
