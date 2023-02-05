#version 330 core

layout(std140) uniform ObjectMatrices{
	mat4 mvp[3]; // Model, View, Proj
};

in vec3 position;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 LightColour;

void main(void){
	gl_Position = vec4(position, 1);
}