#version 150

uniform sampler2D Normals;
uniform sampler2D Positions;
uniform sampler2D NormalMap;
uniform sampler2D Depth;

uniform mat4  modelMatrix;
uniform mat4  viewMatrix;
uniform mat4  projectionMatrix;
uniform float NbSamples;
uniform float SamplingRadius;
uniform float OcclusionBias;
uniform float Scale;

in vec2 uv;

out vec4 Color;
 
float doAmbientOcclusion(in vec2 tcoord, in vec2 uv, in vec3 original, in vec3 cnorm)
{
    vec3 newp = texture2D(Positions, tcoord+uv).rgb;	
    vec3 diff = newp - original;
    vec3 v = normalize(diff);
    float d = length(diff) * Scale;
 
    float ret = max(0.0, dot(cnorm, v) - OcclusionBias) * (1.0 / (1.0 + d));
    return ret;
}

void main(void)
{    	
 	vec2 KERNEL[64];
	KERNEL[0] = vec2(-0.613392, 0.617481);
	KERNEL[1] = vec2(0.170019, -0.040254);
	KERNEL[2] = vec2(-0.299417, 0.791925);
	KERNEL[3] = vec2(0.645680, 0.493210);
	KERNEL[4] = vec2(-0.651784, 0.717887);
	KERNEL[5] = vec2(0.421003, 0.027070);
	KERNEL[6] = vec2(-0.817194, -0.271096);
	KERNEL[7] = vec2(-0.705374, -0.668203);
	KERNEL[8] = vec2(0.977050, -0.108615);
	KERNEL[9] = vec2(0.063326, 0.142369);
	KERNEL[10] = vec2(0.203528, 0.214331);
	KERNEL[11] = vec2(-0.667531, 0.326090);
	KERNEL[12] = vec2(-0.098422, -0.295755);
	KERNEL[13] = vec2(-0.885922, 0.215369);
	KERNEL[14] = vec2(0.566637, 0.605213);
	KERNEL[15] = vec2(0.039766, -0.396100);
	KERNEL[16] = vec2(0.751946, 0.453352);
	KERNEL[17] = vec2(0.078707, -0.715323);
	KERNEL[18] = vec2(-0.075838, -0.529344);
	KERNEL[19] = vec2(0.724479, -0.580798);
	KERNEL[20] = vec2(0.222999, -0.215125);
	KERNEL[21] = vec2(-0.467574, -0.405438);
	KERNEL[22] = vec2(-0.248268, -0.814753);
	KERNEL[23] = vec2(0.354411, -0.887570);
	KERNEL[24] = vec2(0.175817, 0.382366);
	KERNEL[25] = vec2(0.487472, -0.063082);
	KERNEL[26] = vec2(-0.084078, 0.898312);
	KERNEL[27] = vec2(0.488876, -0.783441);
	KERNEL[28] = vec2(0.470016, 0.217933);
	KERNEL[29] = vec2(-0.696890, -0.549791);
	KERNEL[30] = vec2(-0.149693, 0.605762);
	KERNEL[31] = vec2(0.034211, 0.979980);
	KERNEL[32] = vec2(0.503098, -0.308878);
	KERNEL[33] = vec2(-0.016205, -0.872921);
	KERNEL[34] = vec2(0.385784, -0.393902);
	KERNEL[35] = vec2(-0.146886, -0.859249);
	KERNEL[36] = vec2(0.643361, 0.164098);
	KERNEL[37] = vec2(0.634388, -0.049471);
	KERNEL[38] = vec2(-0.688894, 0.007843);
	KERNEL[39] = vec2(0.464034, -0.188818);
	KERNEL[40] = vec2(-0.440840, 0.137486);
	KERNEL[41] = vec2(0.364483, 0.511704);
	KERNEL[42] = vec2(0.034028, 0.325968);
	KERNEL[43] = vec2(0.099094, -0.308023);
	KERNEL[44] = vec2(0.693960, -0.366253);
	KERNEL[45] = vec2(0.678884, -0.204688);
	KERNEL[46] = vec2(0.001801, 0.780328);
	KERNEL[47] = vec2(0.145177, -0.898984);
	KERNEL[48] = vec2(0.062655, -0.611866);
	KERNEL[49] = vec2(0.315226, -0.604297);
	KERNEL[50] = vec2(-0.780145, 0.486251);
	KERNEL[51] = vec2(-0.371868, 0.882138);
	KERNEL[52] = vec2(0.200476, 0.494430);
	KERNEL[53] = vec2(-0.494552, -0.711051);
	KERNEL[54] = vec2(0.612476, 0.705252);
	KERNEL[55] = vec2(-0.578845, -0.768792);
	KERNEL[56] = vec2(-0.772454, -0.090976);
	KERNEL[57] = vec2(0.504440, 0.372295);
	KERNEL[58] = vec2(0.155736, 0.065157);
	KERNEL[59] = vec2(0.391522, 0.849605);
	KERNEL[60] = vec2(-0.620106, -0.328104);
	KERNEL[61] = vec2(0.789239, -0.419965);
	KERNEL[62] = vec2(-0.545396, 0.538133);
	KERNEL[63] = vec2(-0.178564, -0.596057);
 
 	float depth = texture2D(Depth,uv).r;
	vec2  xy = uv * 2.0 -1.0;
	vec4  wPosition = inverse(projectionMatrix*viewMatrix) * vec4(xy, depth * 2.0 -1.0, 1.0);
	vec3  p = vec3(wPosition/wPosition.w);
	
	vec3 n = vec3(normalize(texture2D(Normals, uv).rgb));
	vec4 n2 = vec4(n,1.0);
	n = n2.xyz;
	vec2 rand = normalize(texture2D(NormalMap, 64*64 * uv / 64).xy);
 
	float fColor = 0.0;

	for(int j = 0; j < NbSamples; ++j)
	{
		vec2 coord = reflect(KERNEL[j], rand) * SamplingRadius;
		fColor += doAmbientOcclusion(uv, coord, p, n);
	}

	fColor = fColor / NbSamples;
	Color = vec4(fColor,fColor,fColor,1.0);
}
