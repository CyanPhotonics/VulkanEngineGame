#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec4 outColor;

const int MAX_LIGHTS = 100;

layout(binding = 2) uniform PointLight {
    vec3 positions[MAX_LIGHTS];
    vec3 attenuations[MAX_LIGHTS];
    vec3 colours[MAX_LIGHTS];
    int count;
} pointLight;

layout(binding = 3) uniform sampler2D texSampler;

void main() {

    vec3 light = vec3(0);

    for (int i = 0 ; i < pointLight.count ; i++){

        vec3 toLight = pointLight.positions[i] - fragPos;
        float intensity = dot(normalize(toLight), normalize(normal));
        float dist = length(toLight);
        float dist2 = dot(toLight, toLight);
        intensity *= 1.0f/((pointLight.attenuations[i].x * dist2)+(pointLight.attenuations[i].y * dist)+(pointLight.attenuations[i].z));
        intensity = clamp(intensity, 0, 1);

        light += pointLight.colours[i] * intensity;

    }

    light = clamp(light, 0.01f, 1.0f);

    outColor = texture(texSampler, fragTexCoord) * vec4(light, 1.0f);

}