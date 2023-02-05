#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2D noiseTex;

in Vertex {
	vec4 colour;
} IN;

out vec4 fragColour[2];

void main(void) {
	fragColour[0] = IN.colour;
	fragColour[0].w = 1.0;
}