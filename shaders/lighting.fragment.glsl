#version 150
#extension GL_ARB_explicit_attrib_location : enable

layout(location = 0) out vec3 FragColor;

uniform vec3 camera_position;
uniform sampler2D diffuse_texture;
uniform float lightIntensity;
uniform float distance;

in vec2 uv;
in vec3 position;
in vec3 normal;

vec3 PointLight(vec3 lightPosition, vec3 lightColor, float LightIntensity, float spec, vec3 n, vec3 diffuse)
{
	vec3 l =  lightPosition - position;
	vec3 v = position - camera_position;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);
	float d = length(l);
	float light_coef = 1.0 / (1.0 + (d*d) * 1.5);
	vec3 color = lightColor * light_coef * LightIntensity * (diffuse * n_dot_l + spec);
	return color;
}

void main(void)
{
	vec3 n = normalize(normal);
	vec3 diffuse = texture(diffuse_texture, uv).rgb;
	
	vec3  lightColor = vec3(1.0, 1.0, 1.0);
	float specular = 0;
	
	vec3  lightPosition = vec3(distance, distance, distance);
        vec3 pl1 = PointLight(lightPosition,lightColor,lightIntensity,specular, n, diffuse);
	
	lightPosition = vec3(-distance, distance, distance);
        vec3 pl2 = PointLight(lightPosition,lightColor,lightIntensity,specular, n, diffuse);
	
	lightPosition = vec3(-distance, -distance, distance);
        vec3 pl3 = PointLight(lightPosition,lightColor,lightIntensity,specular, n, diffuse);
	
	lightPosition = vec3(distance, -distance, distance);
        vec3 pl4 = PointLight(lightPosition,lightColor,lightIntensity,specular, n, diffuse);
	
	lightPosition = vec3(distance, distance, -distance);
        vec3 pl5 = PointLight(lightPosition,lightColor,lightIntensity,specular, n, diffuse);
	
	lightPosition = vec3(-distance, distance, -distance);
	vec3 pl6 = PointLight(lightPosition,lightColor,lightIntensity,specular, n, diffuse);
	
	lightPosition = vec3(-distance, -distance, -distance);
	vec3 pl7 = PointLight(lightPosition,lightColor,lightIntensity,specular, n, diffuse);
	
	lightPosition = vec3(distance, -distance, -distance);
	vec3 pl8 = PointLight(lightPosition,lightColor,lightIntensity,specular, n, diffuse);
	
	FragColor = vec3(pl1+pl2+pl3+pl4+pl5+pl6+pl7+pl8);
}
