#version 430

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
out vec3 varyingNormal;
out vec3 varyingLightDir1;
out vec3 varyingLightDir2;
out vec3 varyingVertPos;
out vec3 varyingHalfVector1;
out vec3 varyingHalfVector2;

struct PositionalLight
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position1;
	vec3 position2;
};
struct Material
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

void main(void)
{	varyingVertPos = (mv_matrix * vec4(vertPos,1.0)).xyz;
	varyingLightDir1 = light.position1 - varyingVertPos;
	varyingLightDir2 = light.position2 - varyingVertPos;
	varyingNormal = (norm_matrix * vec4(vertNormal,1.0)).xyz;
	
	varyingHalfVector1 =
		normalize(normalize(varyingLightDir1)
		+ normalize(-varyingVertPos)).xyz;
	varyingHalfVector2 =
		normalize(normalize(varyingLightDir2)
		+ normalize(-varyingVertPos)).xyz;

	gl_Position = proj_matrix * mv_matrix * vec4(vertPos,1.0);
}
