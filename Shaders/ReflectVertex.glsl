#version 330 core

layout(std140) uniform ObjectMatrices{
	mat4 mvp[4]; // Model, View, Proj, Tex
};

uniform vec3 lightPos;

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec4 tangent;

out Vertex{
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
	vec3 tangent;
	vec3 binormal;
} OUT;

void main(void){
	OUT.texCoord = (mvp[3] * vec4(texCoord, 0.0, 1.0)).xy;

	mat3 normalMatrix = transpose(inverse(mat3(mvp[0])));
	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

	OUT.normal = wNormal;
	OUT.tangent = wTangent;
	OUT.binormal = cross(wTangent, wNormal) * tangent.w;

	vec4 worldPos = (mvp[0] * vec4(position, 1));
	OUT.worldPos = worldPos.xyz;

	gl_Position = (mvp[2] * mvp[1]) * worldPos;
}