#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform Transform {
    mat4 mvp;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_tex_coord;

layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec2 frag_tex_coord;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = mvp *  vec4(in_position, 1.0);
    frag_color = in_color;
    frag_tex_coord = in_tex_coord;
}
