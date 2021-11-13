#version 430

in vec3 varyingNormal;
in vec3 varyingLightDir1;
in vec3 varyingLightDir2;
in vec3 varyingVertPos;
in vec3 varyingHalfVector1;
in vec3 varyingHalfVector2;

out vec4 fragColor;

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
{	// normalize the light, normal, and view vectors:
	vec3 L1 = normalize(varyingLightDir1);
	vec3 L2 = normalize(varyingLightDir2);
	vec3 N = normalize(varyingNormal);
	vec3 V = normalize(-varyingVertPos);
	
	// get the angle between the light and surface normal:
	float cosTheta1 = dot(L1,N);
	float cosTheta2 = dot(L2,N);
	
	float diffuse1 = max(cosTheta1,0.0);
	float diffuse2 = max(cosTheta2,0.0);
	// halfway vector varyingHalfVector was computed in the vertex shader,
	// and interpolated prior to reaching the fragment shader.
	// It is copied into variable H here for convenience later.
	vec3 H1 = normalize(varyingHalfVector1);
	vec3 H2 = normalize(varyingHalfVector2);
	
	// get angle between the normal and the halfway vector
	float cosPhi1 = dot(H1,N);
	float cosPhi2 = dot(H2,N);
	
	float specular1 = max(cosPhi1,0.0);
	float specular2 = max(cosPhi2,0.0);
	// compute ADS contributions (per pixel):
	vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
	vec3 diffuse_1 = light.diffuse.xyz * material.diffuse.xyz * diffuse1;
	vec3 specular_1 = light.specular.xyz * material.specular.xyz * pow(specular1, material.shininess*3.0);
	vec3 diffuse_2 = light.diffuse.xyz * material.diffuse.xyz * diffuse2;
	vec3 specular_2 = light.specular.xyz * material.specular.xyz * pow(specular2, material.shininess*3.0);
	fragColor = clamp(vec4((ambient + diffuse_1 + specular_1) * 0.5 + (ambient + diffuse_2 + specular_2) * 0.5, 1.0),0,1);
}
