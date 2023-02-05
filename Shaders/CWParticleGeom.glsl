#version 400 core	

layout(std140) uniform ObjectMatrices{
	mat4 mvp[3]; // Model, View, Proj
};

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out Vertex{
	vec4 colour;
} OUT;

const float particle_size = 50.0f;
mat4 projectionViewMatrix = mvp[2] * mvp[1];

void createVertex(vec3 offset, vec4 originalPosition){
	vec4 actualOffset = vec4(offset * particle_size, 0.0);
	vec4 worldPosition = originalPosition + actualOffset;
	gl_Position = mvp[2] * mvp[1] * mvp[0] * worldPosition;
	OUT.colour = vec4(1, 0, 0, 1);OUT.colour = vec4(0, 0, 1, 1);
	EmitVertex();
}

void main(void){
	createVertex(vec3(-1.0, 1.0, 1.0), gl_in[0].gl_Position);
	createVertex(vec3(-1.0, -1.0, 1.0), gl_in[0].gl_Position);
	createVertex(vec3(1.0, -1.0, 1.0), gl_in[0].gl_Position);
	createVertex(vec3(1.0, -1.0, 1.0), gl_in[0].gl_Position);
	EndPrimitive();
}