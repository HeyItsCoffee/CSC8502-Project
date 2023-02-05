#version 330 core

layout(std140) uniform ObjectMatrices{
	mat4 mvp[3]; // Model, View, Proj
};

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;

void main(void){
	OUT.colour = colour;
	OUT.texCoord = texCoord;

	mat3 normalMatrix = transpose(inverse(mat3(mvp[0])));

	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

	OUT.normal = normalize(normalMatrix * normalize(normal));
	OUT.tangent = wTangent;
	OUT.binormal = cross(wTangent, wNormal) * tangent.w;

	vec4 worldPos = (mvp[0] * vec4(position, 1));

	OUT.worldPos = worldPos.xyz;

	//gl_Position = (mvp[2] * mvp[1]) * worldPos;
	gl_Position = mvp[2] * (mvp[1] * worldPos);
}