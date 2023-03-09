#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;

// TODO(student): Declare various other uniforms
uniform float rotation_time;

// Output
layout(location = 0) out vec4 out_color;

mat4 translation(float diff) {
    return mat4(1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                diff, 1, 1, 1);
}

void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.
    vec4 v = translation(rotation_time) * vec4(texcoord, 1, 1);
    vec2 final_coord = vec2(v);
    
    vec4 coord1 = texture2D(texture_1, final_coord);
    vec4 coord2 = texture2D(texture_2, final_coord);

    out_color = coord1; //mix(coord1, coord2, 0.5f);
}
