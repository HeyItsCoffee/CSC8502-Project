#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform samplerCube cubeTex;

uniform vec3 cameraPos;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
	vec3 tangent;
	vec3 binormal;
} IN;

out vec4 fragColour[2];

void main(void) {
	vec3 viewDir = normalize(cameraPos - IN.worldPos);

	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	vec3 bumpNormal = texture(bumpTex, IN.texCoord).rgb;
	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

	vec3 reflectDir = reflect(-viewDir, normalize(bumpNormal));
	vec4 reflectTex = texture(cubeTex, reflectDir);

	fragColour[0] = reflectTex + (diffuse * 0.3f);
	fragColour[0].w = 0.9;
	fragColour[1] = vec4(bumpNormal.xyz * 0.5 + 0.5, 1.0);
}