#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 textCoords;

out gl_PerVertex {
    vec4 gl_Position;
};

const vec3 positions[6] = vec3[](
    vec3(1.0f,-1.0f,0),
    vec3(-1.0f,-1.0f,0),
    vec3(-1.0f,1.0f,0),

    vec3(1.0f,-1.0f,0),
    vec3(-1.0f,1.0f,0),
    vec3(1.0f,1.0f,0)
);

//const vec3 positions[6] = vec3[](
//    vec3(0.5f,-0.5f,0.5f), vec3(-0.5f,0.5f,0.5f), vec3(-0.5f,0.5f,-0.5f),
//    vec3(-0.5f,-0.5f,-0.5f), vec3(-0.5f,0.5f,0.5f), vec3(-0.5f,0.5f,0.5f)
//);

void main() {

    //gl_Position = vec4(inPosition,1);
    gl_Position = vec4(positions[gl_VertexIndex],1);
    textCoords = (positions[gl_VertexIndex].xy * 0.5 + 0.5) * 0.5;

}