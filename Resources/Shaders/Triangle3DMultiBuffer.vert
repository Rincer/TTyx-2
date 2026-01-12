#version 450

layout(binding = 0) uniform U0 { mat4 model; } modelUbo;
layout(binding = 1) uniform U1 { mat4 view; } viewUbo;
layout(binding = 2) uniform U2 { mat4 proj; } projUbo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = projUbo.proj * viewUbo.view * modelUbo.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}

