#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec3 normal;

//Static
layout(binding = 0) uniform Camera {
    mat4 projectionViewMatrix;
} camera;

//Non-static
layout(binding = 1) uniform Model {
    mat4 modelMatrix;
} model;

void main() {
    vec4 worldPos = model.modelMatrix * vec4(inPosition, 1.0f);
    fragPos = worldPos.xyz;
    //normal = (model.modelMatrix * vec4(inNormal, 1.0f)).xyz;
    normal = (model.modelMatrix * vec4(inNormal,0.0f)).xyz;

    gl_Position = camera.projectionViewMatrix * worldPos;
	fragTexCoord = inTexCoord;
}