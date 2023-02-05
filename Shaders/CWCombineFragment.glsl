#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

uniform vec2 lightStrength;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void){
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	vec3 light = texture(diffuseLight, IN.texCoord).xyz;
	vec3 specular = texture(specularLight, IN.texCoord).xyz;

	if(diffuse.w != 1){
		fragColour.xyz = diffuse.xyz;
		if(lightStrength.x > lightStrength.y){
			fragColour.xyz *= (vec3(0.957f, 0.914f, 0.609f) * max(lightStrength.x, 0.25));
		}
		else{
			fragColour.xyz *= (vec3(0.668f, 0.754f, 0.887f) * max(lightStrength.y, 0.25));
		}
		fragColour.a = 1;
		return;
	}

	fragColour.xyz = diffuse.xyz * 0.1; // Ambient
	fragColour.xyz += diffuse.xyz * light; // Lambert
	fragColour.xyz += specular; // specular
	fragColour.a = 1.0;
}