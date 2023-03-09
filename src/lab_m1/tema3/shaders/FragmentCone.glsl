#version 330

uniform vec3 object_color;

// Output
layout(location = 0) out vec4 out_color;

void main()
{

    out_color = vec4(1);
    out_color *= vec4(object_color, 0.3f);
}
