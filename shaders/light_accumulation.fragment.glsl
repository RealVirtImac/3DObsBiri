#version 150
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_gpu_shader5 : enable

uniform vec3 camera_position;
uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 light_direction;
uniform float light_intensity;
uniform sampler2D material_texture;
uniform sampler2D normal_texture;
uniform sampler2D depth_texture;
uniform sampler2D shadowMap;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 light_projection;

in vec2 uv;

out vec4 out_frag_color;

vec3 spotLight(in vec3 lcolor, in float intensity, in vec3 ldir, in vec3 lpos, in vec3 n, in vec3 fpos, vec3 diffuse, float spec, vec3 cpos)
{
	vec3 l =  lpos - fpos;
	float cosTs = dot( normalize(-l), normalize(ldir) ); 
	float thetaP =  radians(30.0);
	float cosTp = cos(thetaP);      
	vec3 v = fpos - cpos;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);
	float d = distance(l, fpos);
	vec3 color = vec3(0.0, 0.0, 0.0);
	if (cosTs > cosTp) 
		color = pow(cosTs, 30.0) * lcolor * intensity * (diffuse * n_dot_l + spec * vec3(1.0, 1.0, 1.0) *  pow(n_dot_h, spec * 100.0));
	return color;
}

void main(void)
{
	vec3  material = texture(material_texture, uv).rgb;
	vec3  normal = texture(normal_texture, uv).rgb;
	float depth = texture(depth_texture, uv).r;
	float shadow = texture(shadowMap,uv).r;
	
	vec2  xy = uv * 2.0 -1.0;
	vec4  wPosition = inverse(projection_matrix*view_matrix) * vec4(xy, depth * 2.0 -1.0, 1.0);
	vec3  position = vec3(wPosition/wPosition.w);
	
	vec4 wlightSpacePosition = light_projection * vec4(position, 1.0);
	vec3 lightSpacePosition = vec3(wlightSpacePosition/wlightSpacePosition.w);
	
	vec3 diffuse = material.rgb;
	float spec = 1.0;

	vec3 n = normalize(normal);
	vec3 l =  light_position - position;

	vec3 v = position - camera_position;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);

	float d = distance(light_position, position);
	float att = clamp(1.0 / ( 1.0 + 1.0 * (d*d)), 0.0, 1.0);

	vec3 color = light_color * light_intensity * att * (diffuse * n_dot_l + spec * vec3(1.0, 1.0, 1.0) *  pow(n_dot_h, spec * 100.0));
	out_frag_color = vec4(color,1.0);
}
