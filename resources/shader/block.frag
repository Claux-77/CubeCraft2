#version 330 core
in vec3 frag_lighting;
in vec3 frag_pos;
in vec3 frag_texcoord;

out vec4 color;
uniform sampler2DArray sampler;
uniform sampler2D skySampler;
uniform vec3 camera;
uniform float viewDistance;
uniform float Time;

float fog_factor;
float fog_height;

const float pi = 3.14159265;

void main()
{
	color = texture(sampler, frag_texcoord);
	if(color.a == 0.0f)
		discard;

	//fog
	float camera_distance = distance(camera, frag_pos);
	fog_factor = pow(clamp(camera_distance / viewDistance, 0.0, 1.0), 4.0);
	float dy = frag_pos.y - camera.y;
	float dx = distance(frag_pos.xz, camera.xz);
	fog_height = (atan(dy, dx) + pi / 2) / pi;

    vec3 sky_color = vec3(texture2D(skySampler, vec2(Time, 1.0f - fog_height)));

	vec3 color3 = color.rgb;
	color3 *= frag_lighting.x * frag_lighting.y * frag_lighting.z;
	//gamma correction
	float gamma = 0.8;
	color3 = pow(color3, vec3(1.0 / gamma));

	color3 = mix(color3, sky_color, fog_factor);
	color = vec4(color3, color.a);
}
