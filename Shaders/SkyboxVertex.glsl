#version 330 core

layout(std140) uniform ObjectMatrices{
	mat4 mvp[3]; // Model, View, Proj
};

in vec3 position;

out Vertex{
	vec3 viewDir;
} OUT;

void main(void){
	vec3 pos = position;
	mat4 invproj = inverse(mvp[2]);
	pos.xy *= vec2(invproj[0][0], invproj[1][1]);
	pos.z = -1.0f;
	OUT.viewDir = transpose(mat3(mvp[1])) * normalize(pos);
	gl_Position = vec4(position, 1.0);
}