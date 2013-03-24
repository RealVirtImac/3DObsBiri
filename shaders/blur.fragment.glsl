#version 150
#extension GL_ARB_explicit_attrib_location : enable

uniform sampler2D TextureToBlur;
uniform float BlurCoef;

in vec2 uv;

out vec4  OutColor;

void main(void)
{
    ivec2 position = ivec2(gl_FragCoord.x,gl_FragCoord.y);
    vec4 input = texelFetch(TextureToBlur,position,0);

    vec3 color = vec3(0.0,0.0,0.0);

    for (int i=0; i<BlurCoef; i++)
    for (int j=0; j<BlurCoef; j++) {
            vec3 sample  = texelFetch( TextureToBlur, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
            color += sample;
    }

    color /= BlurCoef*BlurCoef;
	
    OutColor = vec4(color, 1.0);
}
