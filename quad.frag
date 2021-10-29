#version 410

in vec2 texture_coord;
out vec4 frag_color;

uniform sampler2D texture_to_draw;

//i don' tthink this is loading in properly






void main() {

     frag_color = texture(texture_to_draw, texture_coord);
}