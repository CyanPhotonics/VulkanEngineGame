#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 textCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;

layout(binding = 1) uniform BlurData {

    uint imageWidth;
    uint imageHeight;

    float resScale;

    float temp;

} blurData;

const float[11] weights = {
    0.000003, 0.000229, 0.005977, 0.060598, 0.24173, 0.382925, 0.24173, 0.060598, 0.005977, 0.000229, 0.000003
};

void main() {

    vec3 colourSum = vec3(0);

    for (int i = -5 ; i <= 5 ; i++){
        colourSum += texture(texSampler,  vec2(textCoords.x, clamp(textCoords.y + i,0,blurData.imageHeight*blurData.resScale-2))).xyz * weights[i+5];
    }

    outColor = vec4(colourSum, 1);

}