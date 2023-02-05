#version 410 core

layout (vertices=3) out; // num vertices in patch output;

const float tessLevelInner = 2;
const float tessLevelOuter = 2;


void main(){
	gl_TessLevelInner[0] = tessLevelInner;

	gl_TessLevelOuter[0] = tessLevelOuter;
	gl_TessLevelOuter[1] = tessLevelOuter;
	gl_TessLevelOuter[2] = tessLevelOuter;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}