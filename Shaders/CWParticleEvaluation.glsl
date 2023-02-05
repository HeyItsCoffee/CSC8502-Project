#version 400 core

layout (triangles, cw) in;

layout(std140) uniform ObjectMatrices{
	mat4 mvp[3]; // Model, View, Proj
};

vec4 TriMixVec4(vec4 a, vec4 b, vec4 c){
	vec4 p0 = gl_TessCoord.x * a;
	vec4 p1 = gl_TessCoord.y * c;
	vec4 p2 = gl_TessCoord.z * b;

	return p0 + p1 + p2;
}

void main(){
	vec4 combinedPos = TriMixVec4(gl_in[0].gl_Position,
									gl_in[1].gl_Position,
									gl_in[2].gl_Position);
	gl_Position = mvp[2] * mvp[1] * combinedPos;
}