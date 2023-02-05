#version 330 core

layout(std140) uniform ObjectMatrices{
	mat4 mvp[3]; // Model, View, Proj
};

in vec3 position;

void main(void){
	gl_Position = (mvp[2] * mvp[1] * mvp[0]) * vec4(position, 1.0);
}