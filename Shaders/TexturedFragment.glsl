#version 330 core
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	//vec4 colour;
} IN;

out vec4 fragColour;
void main(void) {
	//fragColour = mix(texture(diffuseTex, IN.texCoord),IN.colour,0.5);
	fragColour = texture(diffuseTex, IN.texCoord);
	//fragColour = IN.colour;
}