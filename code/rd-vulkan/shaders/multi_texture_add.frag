#version 450

#extension GL_EXT_nonuniform_qualifier : enable
// Global bindless support. This should go in a common file.

layout(push_constant) uniform Transform {
    mat4 mvp;
    float tex_idx_0;
    float tex_idx_1;
};

layout ( set = 0, binding = 0 ) uniform sampler2D global_textures[];

layout(location = 0) in vec4 frag_color;
layout(location = 1) in vec2 frag_tex_coord0;
layout(location = 2) in vec2 frag_tex_coord1;

layout(location = 0) out vec4 out_color;

layout (constant_id = 0) const int alpha_test_func = 0;

void main() {
    int frag_tex_idx0 = floatBitsToInt(tex_idx_0);
    int frag_tex_idx1 = floatBitsToInt(tex_idx_1);

    vec4 color_a = frag_color * texture(global_textures[nonuniformEXT(frag_tex_idx0)], frag_tex_coord0);
    vec4 color_b = texture(global_textures[nonuniformEXT(frag_tex_idx1)], frag_tex_coord1);
    out_color = vec4(color_a.rgb + color_b.rgb, color_a.a * color_b.a);

    if (alpha_test_func == 1) {
        if (out_color.a == 0.0f) discard;
    } else if (alpha_test_func == 2) {
        if (out_color.a >= 0.5f) discard;
    } else if (alpha_test_func == 3) {
        if (out_color.a < 0.5f) discard;
    } else if (alpha_test_func == 3) {
        if (out_color.a < 0.75f) discard;
    }
}
