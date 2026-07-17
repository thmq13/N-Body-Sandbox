#pragma once

#include <Core/Message.hpp>

namespace SchemaRenderer {
    void drawInput(ParameterSchema& schema, float fieldWidth = 100.0f);
    void drawDisplay(const ParameterSchema& schema);
}