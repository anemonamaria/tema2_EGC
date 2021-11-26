#version 330

// Input
// TODO(student): Get values from vertex shader
in vec3 v_position;
in vec3 v_normal;
in vec2 v_texture;
in vec3 v_color;

// Output
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec3 out_texture;

void main()
{
    // TODO(student): Write pixel out color
    out_color	= vec4(v_color, 1.5f);
	out_normal	= vec4(v_normal, 1.f);
	out_texture = vec3(v_texture, 1.f);
}
