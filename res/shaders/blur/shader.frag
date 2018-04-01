#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 textCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;

layout(binding = 1) uniform BlurData {

    ivec4 data;

} blurData;

//TODO make gaussian and make faster

void main() {

    // TODO scale based on resolution or higher blur range, currently for half resolution
    vec2 iter = 0.5f/blurData.data.xy;
    int size = blurData.data.z;

    vec2 iOffset = (size-1)/2 * iter;

    vec3 colourSum = vec3(0);

    for (int x = 0 ; x < size ; x++){
        for(int y = 0 ; y < size ; y++){

            colourSum += texture(texSampler, textCoords - iOffset + (iter * vec2(x, y))).xyz;

        }
    }

    outColor = vec4(colourSum / pow(size,2), 1);

}