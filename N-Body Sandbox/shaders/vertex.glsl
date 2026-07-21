#version 330 core

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

in mat4 instanceTransform;
in vec4 instanceColor;

uniform mat4 mvp;

out vec4 fragColor;

void main() {
   
    fragColor = instanceColor;

    gl_Position = mvp * instanceTransform * vec4(vertexPosition, 1.0);
}