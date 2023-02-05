#version 330 core

layout(std140) uniform ObjectMatrices{
	mat4 mvp[5]; // Model, View, Proj, Tex, Shad
};

uniform vec3 lightPos;

in vec3 position;
in vec3 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex{
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
} OUT;

void main(void){
	OUT.colour = colour;
	OUT.texCoord = texCoord;
	mat3 normalMatrix = transpose(inverse(mat3(mvp[0])));
	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

	OUT.normal = normal;
	OUT.tangent = wTangent;
	OUT.binormal = cross(wNormal, wTangent) * tangent.w;

	vec4 worldPos = (mvp[0] * vec4(position, 1));
	OUT.worldPos = worldPos.xyz;
	gl_Position = (mvp[2] * mvp[1]) * worldPos;

	// New shadow stuff
	vec3 viewDir = normalize(lightPos - worldPos.xyz);
	vec4 pushVal = vec4(OUT.normal, 0) * dot(viewDir, OUT.normal);
	OUT.shadowProj = mvp[4] * (worldPos + pushVal);
}