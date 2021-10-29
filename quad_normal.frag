#version 410

in vec2 texture_coord;
out vec4 frag_color;

uniform sampler2D texture_to_draw;

//i don' tthink this is loading in properly
uniform sampler2D texture_normal;

uniform vec3 light;



void main() {
     //how to light this guy
     vec2 st = gl_FragCoord.xy;
     st.x /= 1280.0f;
     st.y /= 720.0f;
     vec3 pixel_3d = vec3(st.x, st.y, 1.0f);//should come from normal map
     vec4 normal_value_texture = texture(texture_normal, texture_coord);
     vec3 normal_value_vec = vec3(normal_value_texture.r, normal_value_texture.g, normal_value_texture.b);
     vec3 light_direction = light - pixel_3d;
     light_direction = normalize(light_direction);
     float dot_prod = dot(normal_value_vec, light_direction);

     if (length(light) > 0.0f) {
     	//frag_color = texture(texture_normal, texture_coord);
     	frag_color = texture(texture_to_draw, texture_coord);
	frag_color.r = frag_color.r*dot_prod;
	frag_color.g = frag_color.g*dot_prod;
	frag_color.b = frag_color.b*dot_prod;
	//frag_color.r = frag_color.r*light.x;
	//frag_color.g = frag_color.g*light.y;
	//frag_color.b = frag_color.b*light.z;
     } else {

       //frag_color = texture(texture_to_draw, texture_coord);
     }
}