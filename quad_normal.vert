#version 410

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 texture_coord_in;

out vec2 texture_coord;

uniform mat4 perspective;
uniform mat4 transform;

void main() {
     gl_Position = perspective * transform * vec4(vertex_position, 1.0);
     texture_coord = texture_coord_in;
}